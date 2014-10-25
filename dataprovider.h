#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include <QtNetwork>
#include <QNetworkDiskCache>
#include <QIcon>

typedef QHash<QString, QString> ChannelInfo;
typedef QHash<QString, ChannelInfo> ChannelData;
typedef QList<ChannelInfo> ChannelList;

class DataProvider : public QObject
{
    Q_OBJECT
public:
    explicit DataProvider(QObject *parent = 0);
    QUrl getPlaylistUrl(QString);

private:
    QNetworkAccessManager *m_networkManager;
    QUrl m_channelsUrl;
    QHash<QUrl, QString> m_iconRequests;
    QHash<QUrl, QString> m_imageRequests;
    QNetworkDiskCache *m_diskCache;
    QString m_cacheDir;
    ChannelData m_channelData;
    void sendRequest(QUrl);

signals:
    void channelListProvided(ChannelList);
    void channelIconProvided(QString, QPixmap);
    void channelImageProvided(QString, QImage);

public slots:
    void handleNetworkReply(QNetworkReply*);
    void parseChannelsXml(QByteArray);
    void provideChannelList();
    void provideChannelIcon(QString);
    void provideChannelImage(QString id);

};

#endif // DATAPROVIDER_H
