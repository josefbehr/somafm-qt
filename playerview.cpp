#include "playerview.h"
#include <QDebug>

PlayerView::PlayerView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMaximumHeight(100);
    setCacheMode(QGraphicsView::CacheBackground);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QGraphicsScene *scene = new QGraphicsScene;
    m_mainText = scene->addSimpleText(tr("Not playing"));
    m_mainText->setFont(QFont("", 12, QFont::Bold, false));
    m_mainText->setBrush(QBrush(Qt::black));
    m_mainText->setPen(QPen(QBrush(Qt::white), 0.5, Qt::SolidLine, Qt::RoundCap));
    setScene(scene);
    m_player = new QMediaPlayer;
    m_playlist = new QMediaPlaylist;
    m_player->setPlaylist(m_playlist);
    connect(m_playlist, SIGNAL(loaded()),
            m_player, SLOT(play()));
    connect(m_player, SIGNAL(bufferStatusChanged(int)),
            this, SLOT(showBufferStatus(int)));
    connect(m_player, SIGNAL(metaDataChanged()),
            this, SLOT(showMetaData()));
}

void PlayerView::showMetaData() {
    qDebug() << "PlayerView::showMetaData";
    QStringList avail = m_player->availableMetaData();
    qDebug() << avail;
    if(avail.contains("Title")) {
        m_mainText->setText(m_player->metaData("Title").toString());
    }
}

void PlayerView::showBufferStatus(int percent) {
    qDebug() << "PlayerView::showBufferStatus";

    QString text = tr("Buffering ") + QString::number(percent) + "%";
    m_mainText->setText(text);
    qDebug() << m_mainText->boundingRect();
}

void PlayerView::play(QString id, QUrl url) {
    qDebug() << "PlayerView::play";

    if(id.isEmpty() || url.isEmpty()) {
        return;
    }

    if(m_player->state() == QMediaPlayer::PlayingState) {
        m_player->stop();
    }
    m_playlist->clear();
    m_playlist->load(url);
    requestChannelImage(id);
}

void PlayerView::updateChannelImage(QString, QImage image) {
    qDebug() << "PlayerView::updateChannelImage";

    m_channelImage = image;
    resetCachedContent();
    viewport()->update();
}

void PlayerView::drawBackground(QPainter *painter, const QRectF &rect) {
    qDebug() << "PlayerView::drawBackground";

    painter->save();
    if(!m_channelImage.isNull()) {
        QRectF source(m_channelImage.rect());
        //QRectF source(0, 0, width(), height());
        painter->setOpacity(0.25);
        painter->drawImage(rect, m_channelImage, source);
    } else {
        painter->fillRect(rect, QBrush(Qt::gray));
    }
    painter->restore();
}
