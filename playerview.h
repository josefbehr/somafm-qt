#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QGraphicsView>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>

class PlayerView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PlayerView(QWidget *parent = 0);

private:
    QImage m_channelImage;
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QGraphicsTextItem *m_mainText;
    QGraphicsRectItem *m_stopButton;
    QGraphicsPolygonItem *m_playButton;
    QString m_id;

protected:
    void setText(QString);

signals:
    void requestChannelImage(QString);
    void playerStarted(QString);
    void playerStopped();

public slots:
    void drawBackground(QPainter*, const QRectF&);
    void updateChannelImage(QString, QImage);
    void play(QString, QUrl);
    void showBufferStatus(int);
    void showMetaData();
    void handleStateChange(QMediaPlayer::State);
    void mousePressEvent(QMouseEvent*);
    void mediaStatusChanged(QMediaPlayer::MediaStatus);
    void handleError(QMediaPlayer::Error);

};

#endif // PLAYERVIEW_H
