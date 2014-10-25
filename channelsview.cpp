#include <QStandardItemModel>
#include <QPainter>
#include <QMenu>

#include "channelsview.h"

void ChannelsViewItemDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index ) const
{
    // do the default drawing
    this->QItemDelegate::paint(painter, option, index);
    qreal width = 1.0;
    QColor color = QColor(Qt::lightGray);
    Qt::PenStyle penstyle = Qt::SolidLine;

    // if not last item, draw line at bottom
    int rows = index.model()->rowCount()-1;
    if(index.row() <= rows) {
        painter->save();
        painter->setPen(QPen(QBrush(color), width, penstyle));
        painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
        painter->restore();
    }
}


ChannelsView::ChannelsView(QWidget *parent) :
    QTableView(parent)
{
    qDebug() << "ChannelsView::ChannelsView";

    setItemDelegate(new ChannelsViewItemDelegate());

    m_columns = QStringList() << "title" << "description";
    m_favoritesColor = QColor(154, 205, 50, 100); // yellowgreen, alpha = 100
    m_rowHeight = fontMetrics().height()*4;

    QHeaderView *verticalHeader = this->verticalHeader();
    verticalHeader->setDefaultSectionSize(m_rowHeight+1);
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->hide();
    horizontalHeader()->hide();
    horizontalHeader()->setStretchLastSection(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setFocusPolicy(Qt::StrongFocus);
    setShowGrid(false);
    setTabKeyNavigation(false);

    connect(this, SIGNAL(activated(QModelIndex)),
            this, SLOT(channelActivated(QModelIndex)));

    setStyleSheet("*{selection-background-color:#ddd;selection-color:#222;}");

    QSettings settings;
    m_favorites = settings.value("favorites", QStringList()).toStringList();
}

ChannelsView::~ChannelsView() {
    QSettings settings;
    settings.setValue("favorites", m_favorites);
}

void ChannelsView::contextMenuEvent(QContextMenuEvent * event) {
    qDebug() << "ChannelsView::contextMenuEvent";

    QIcon favIcon = QIcon::fromTheme("emblem-favorite");
    QString favText;
    if(m_favorites.contains(m_selectedId)) {
        favIcon = QIcon(favIcon.pixmap(m_rowHeight, QIcon::Normal));
        favText = tr("Remove from favorites");
    } else {
        favIcon = QIcon(favIcon.pixmap(m_rowHeight, QIcon::Disabled));
        favText = tr("Add to favorites");
    }
    QMenu *menu = new QMenu(this);
    QAction *favAction = new QAction(favIcon, favText, this);
    menu->addAction(favAction);
    connect(favAction, SIGNAL(triggered()),
            this, SLOT(toggleFavorite()));
    menu->popup(event->globalPos());
}

void ChannelsView::channelActivated(QModelIndex index) {
    qDebug() << "ChannelsView::channelActivated";
    QString id = m_idList.at(index.row());
    emit channelActivated(id);
}

void ChannelsView::selectionChanged(const QItemSelection &selected,
                                    const QItemSelection &deselected) {
    qDebug() << "ChannelsView::selectionChanged";

    if(selected.isEmpty()) { // de-selection only
        m_selectedId.clear();
    } else { // save selected channel id
        // used for restoring selection on update and toggling favorite
        int selectedRow = selected.indexes().first().row();
        QString selectedId = m_idList.at(selectedRow);
        m_selectedId = selectedId;
    }

    QTableView::selectionChanged(selected, deselected);
}

void ChannelsView::toggleFavorite() {
    qDebug() << "ChannelsView::toggleFavorite";
    if(m_favorites.contains(m_selectedId)) {
        m_favorites.removeOne(m_selectedId);
    } else {
        m_favorites.append(m_selectedId);
    }
    updateChannelList(m_channelList); // re-order list and model
}

void ChannelsView::updateChannelList(ChannelList newList) {
    qDebug() << "ChannelsView::updateChannelList";

    // re-sort to have favorites on top and marked as such
    ChannelList channelList, tmpList;
    ChannelList::const_iterator it;
    for(it = newList.constBegin(); it != newList.constEnd(); ++it) {
        ChannelInfo channelInfo(*it);
        QString id = channelInfo.value("id");
        if(m_favorites.contains(id)) {
            channelInfo.insert("favorite", "yes");
            channelList.append(channelInfo);
        } else {
            channelInfo.insert("favorite", "no");
            tmpList.append(channelInfo);
        }
    }
    channelList.append(tmpList);

    // Create model for the table from re-sorted list
    QStandardItemModel *model = new QStandardItemModel;
    model->setColumnCount(m_columns.size());
    //int r = model->rowCount();

    int r;
    QStringList idList;
    for(it = channelList.constBegin(), r = 0;
        it != channelList.constEnd();
        ++it , ++r) {

        QString id = (*it).value("id");
        QString genre = (*it).value("genre");
        QString dj = (*it).value("dj");

        for(int i = 0; i < m_columns.size(); ++i) {
            QString col = m_columns.at(i);
            QStandardItem *item = new QStandardItem((*it).value(col));
            if(col == "title") {
                item->setData(channelIcon(id), Qt::DecorationRole);
                item->setFont(QFont("", -1, QFont::Bold));
                item->setTextAlignment(Qt::AlignCenter);
            }
            if((*it).value("favorite") == "yes") {
                item->setBackground(m_favoritesColor);
            }
            item->setEditable(false);
            item->setToolTip("genre: " + genre +
                             "\ndj: " + (dj.isEmpty() ? "-" : dj));
            model->setItem(r, i, item);
        }
        idList.append(id);
    }
    setModel(model);
    setColumnWidth(0, 200);

    m_idList = idList;
    m_channelList = channelList;

    // conveniently restore selection on channel,
    // has to happen after update of member variables
    // since selectionChanged uses m_idList to save new selected id
    if(!m_selectedId.isEmpty()) {
        selectRow(m_idList.indexOf(m_selectedId));
    }
}

// delivers memory-cached (QHash) pixmaps for channel icons
// and requests them, if not available
QPixmap ChannelsView::channelIcon(QString id) {
    qDebug() << "ChannelsView::channelIcon";

    if(!m_iconList.contains(id)) { // icon not cached
        emit requestChannelIcon(id); // request it
        QPixmap emptyPixmap = QPixmap(m_rowHeight, m_rowHeight);
        emptyPixmap.fill(Qt::transparent);
        return emptyPixmap; // and return transparent icon for now
    }

    return m_iconList.value(id); // otherwise, deliver stored icon
}

void ChannelsView::updateChannelIcon(QString id, QPixmap pixmap) {
    qDebug() << "ChannelsView::updateChannelIcon";

    if(!m_idList.contains(id)) {
        return;
    }
    int row = m_idList.indexOf(id);

    QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model());
    if(!m->hasIndex(row, 0)) {
        return; // this should never happen due to above check, nonetheless...
    }
    QPixmap newIcon = pixmap.scaled(m_rowHeight,
                                    m_rowHeight,
                                    Qt::KeepAspectRatioByExpanding);
    m->setData(m->index(row, 0),
               newIcon,
               Qt::DecorationRole);
    m_iconList.insert(id, newIcon);
}
