#ifndef CHANNELSVIEW_H
#define CHANNELSVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QKeyEvent>
#include <QItemDelegate>

#include "dataprovider.h"

// DO NOT draw focus indicator outlines on any of the list items,
// we use SelectionBehavior::SelectRow and SelectionModel::SingleSelection to
// highlight the whole current row, which displays information for one channel
// paint() is re-implemented to add a small line at the bottom of view items
class ChannelsViewItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    virtual void drawFocus(QPainter*, const QStyleOptionViewItem&, const QRect&) const {}
    void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

class ChannelsView : public QTableView
{
    Q_OBJECT
public:
    explicit ChannelsView(QWidget *parent = 0);
    ~ChannelsView();

private:
    QStringList m_columns;
    int m_rowHeight;
    ChannelList m_channelList;
    QHash<QString, QImage> m_iconCache;
    QStringList m_favorites;
    QStringList m_idList;
    QString m_selectedId;
    QString m_playingId;
    QColor m_favoritesColor;

protected:
    void keyReleaseEvent(QKeyEvent *event);
    QImage channelIcon(QString id);

signals:
    void requestChannelIcon(QString);
    void channelActivated(QString);
    void requestChannelList();

public slots:
    void updateChannelList(ChannelList);
    void updateChannelIcon(QString, QImage);
    void selectionChanged(const QItemSelection&, const QItemSelection&);
    void channelActivated(QModelIndex);
    void contextMenuEvent(QContextMenuEvent*);
    void toggleFavorite();
    void playingState(QString);
    void stoppedState();

};

#endif // CHANNELSVIEW_H
