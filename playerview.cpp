#include "playerview.h"
#include <QDebug>
#include <QMouseEvent>

PlayerView::PlayerView(QWidget *parent) :
    QGraphicsView(parent)
{
    setMaximumHeight(100);
    setCacheMode(QGraphicsView::CacheBackground);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *scene = new QGraphicsScene;
    setScene(scene);

    m_stopButton = scene->addRect(0, 0, 50, 50,
                                  QPen(QBrush(Qt::white),
                                       2.0,
                                       Qt::SolidLine,
                                       Qt::RoundCap,
                                       Qt::RoundJoin),
                                  QBrush(Qt::darkRed));
    m_stopButton->setOpacity(0.5);
    m_stopButton->setPos(-80, -10);
    m_stopButton->hide();

    QPolygonF polygon;
    polygon << QPointF(0, 50) << QPointF(50, 25) << QPointF(0, 0);
    m_playButton = scene->addPolygon(polygon,
                                     QPen(QBrush(Qt::white),
                                          2.0,
                                          Qt::SolidLine,
                                          Qt::RoundCap,
                                          Qt::RoundJoin),
                                     QBrush(Qt::darkGreen));
    m_playButton->setPos(-80, -10);
    m_playButton->setOpacity(0.5);
    m_playButton->hide();

    m_mainText = scene->addText("");
    m_mainText->setFont(QFont(QFont(fontInfo().family(),
                                    fontInfo().pointSize(),
                                    QFont::Bold)));

    setSceneRect(-100, -35, 520, 100);
    setText(tr("Double-click a channel to start playing"));

    m_playlist = new QMediaPlaylist(this);
    m_player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);
    m_player->setPlaylist(m_playlist);

    connect(m_playlist, SIGNAL(loaded()),
            m_player, SLOT(play()));

    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(handleStateChange(QMediaPlayer::State)));

    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(m_player, SIGNAL(error(QMediaPlayer::Error)),
            this, SLOT(handleError(QMediaPlayer::Error)));
}

void PlayerView::setText(QString text) {
    m_mainText->setHtml("<center></center>");
    m_mainText->adjustSize();
    m_mainText->setPos(0, 0);
    m_mainText->setHtml("<center>"+text+"</center>");
    m_mainText->setTextWidth(300);
    centerOn(0, 0);
}

void PlayerView::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton) {
        if(m_stopButton->isVisible() &&
                m_stopButton->isUnderMouse()) {
            m_player->stop();
            m_stopButton->hide();
            m_playButton->show();
            return; // important, otherwise next if-block will validate to true
        }
        if(m_playButton->isVisible() &&
                m_playButton->isUnderMouse()) {
            m_player->play();
            m_playButton->hide();
            m_stopButton->show();
            return;
        }
    }
}

void PlayerView::mediaStatusChanged(QMediaPlayer::MediaStatus status) {
    qDebug() << "PlayerView::mediaStatusChanged" << status;

    switch(status) {
    case QMediaPlayer::LoadingMedia: // buffering, not playing
        connect(m_player, SIGNAL(bufferStatusChanged(int)),
                this, SLOT(showBufferStatus(int)));
        break;
    case QMediaPlayer::BufferedMedia:
        disconnect(m_player, SIGNAL(bufferStatusChanged(int)),
                   this, SLOT(showBufferStatus(int)));
        showMetaData();
        connect(m_player, SIGNAL(metaDataChanged()),
                this, SLOT(showMetaData()));
        m_player->play();
        break;
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::EndOfMedia:
    case QMediaPlayer::InvalidMedia:
        m_player->stop();
        break;
    case QMediaPlayer::StalledMedia:
        m_player->pause();
        break;
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::LoadedMedia:
        break;
    }

}

void PlayerView::handleError(QMediaPlayer::Error error) {
    qDebug() << "PlayerView::handleError" << error;

    switch(error) {
    case QMediaPlayer::NoError:
    case QMediaPlayer::ResourceError:
    case QMediaPlayer::FormatError:
    case QMediaPlayer::NetworkError:
    case QMediaPlayer::AccessDeniedError:
    case QMediaPlayer::ServiceMissingError:
        setText(tr("An error occured"));
        break;
    }
}

void PlayerView::handleStateChange(QMediaPlayer::State state) {
    qDebug() << "PlayerView::handleStateChange" << state;

    switch(state) {
    case QMediaPlayer::PlayingState:
        m_playButton->hide();
        m_stopButton->show();
        break;
    case QMediaPlayer::StoppedState:
        disconnect(m_player, SIGNAL(metaDataChanged()),
                   this, SLOT(showMetaData()));
        disconnect(m_player, SIGNAL(bufferStatusChanged(int)),
                   this, SLOT(showBufferStatus(int)));
        setText(tr("Stopped"));
        break;
    case QMediaPlayer::PausedState: // we don't pause a radio stream
    default:
        break;
    }
}

void PlayerView::showMetaData() {
    qDebug() << "PlayerView::showMetaData";

    QStringList avail = m_player->availableMetaData();
    //for(int i=0; i<avail.size(); ++i) {
    //    qDebug() << avail.at(i) << " : " << m_player->metaData(avail.at(i));
    //}
    if(avail.contains("Title")) {
        setText(m_player->metaData("Title").toString());
    }
}

void PlayerView::showBufferStatus(int percent) {
    //qDebug() << "PlayerView::showBufferStatus";

    QString text = tr("Buffering ") + QString::number(percent) + "%";
    setText(text);
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
    resetCachedContent(); // flush cached background pixmap
    viewport()->update(); // calls drawBackground()
}

void PlayerView::drawBackground(QPainter *painter, const QRectF &rect) {
    qDebug() << "PlayerView::drawBackground";

    painter->save();
    if(!m_channelImage.isNull()) {
        QRectF source(m_channelImage.rect());
        painter->setOpacity(0.25);
        painter->drawImage(rect, m_channelImage, source);
    } else {
        painter->fillRect(rect, QBrush(Qt::gray));
    }
    painter->restore();
}
