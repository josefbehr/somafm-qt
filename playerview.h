#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QGraphicsView>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QGraphicsSimpleTextItem>

class PlayerView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PlayerView(QWidget *parent = 0);

private:
    QImage m_channelImage;
    QMediaPlayer *m_player;
    QMediaPlaylist *m_playlist;
    QGraphicsSimpleTextItem *m_mainText;

signals:
    void requestChannelImage(QString);

public slots:
    void drawBackground(QPainter*, const QRectF&);
    void updateChannelImage(QString, QImage);
    void play(QString, QUrl);
    void showBufferStatus(int);
    void showMetaData();

};

#endif // PLAYERVIEW_H
