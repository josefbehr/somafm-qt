#include <QDebug>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

#include "dataprovider.h"

DataProvider::DataProvider(QObject *parent) :
    QObject(parent)
{
    qDebug() << "DataProvider::DataProvider";

    m_networkManager = new QNetworkAccessManager(this);
    m_diskCache = new QNetworkDiskCache(this);

    m_channelsUrl = QUrl("http://somafm.com/channels.xml");
    //m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    m_cacheDir = "/tmp/somafm-qt-cache-test/"; // TODO CHANGEME
    m_diskCache->setCacheDirectory(m_cacheDir);
    m_networkManager->setCache(m_diskCache);

    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleNetworkReply(QNetworkReply*)));
}

void DataProvider::sendRequest(QUrl url) {
    qDebug() << "DataProvider::sendRequest(" << url.toString() << ")";
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                     QNetworkRequest::PreferCache);
    req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    m_networkManager->get(req);
}

void DataProvider::provideChannelList() {
    qDebug() << "DataProvider::provideChannelList";

    // first, look for the file in the cache manually
    // and use it to provide fast setup at start
    // since the networkmanager will always fetch the
    // remote file due to the received Last-Modified header
    QIODevice *cacheDevice = m_diskCache->data(m_channelsUrl);
    if(cacheDevice != 0) { // file was found in cache
        parseChannelsXml(cacheDevice->readAll());
        cacheDevice->deleteLater();
    }
    sendRequest(m_channelsUrl);
}

void DataProvider::provideChannelIcon(QString id) {
    qDebug() << "DataProvider::provideChannelIcon";

    if(m_channelData.contains(id)) {
        QUrl imageUrl(m_channelData.value(id).value("image"));
        m_iconRequests.insert(imageUrl, id);
        sendRequest(imageUrl);
    }
}

void DataProvider::provideChannelImage(QString id) {
    qDebug() << "DataProvider::provideChannelImage";

    if(m_channelData.contains(id)) {
        ChannelInfo chan = m_channelData.value(id);
        QStringList keys = QStringList() << "xlimage"
                                         << "largeimage"
                                         << "image";
        for(int i=0; i<keys.size(); ++i) {
            QString key = keys.at(i);
            if(chan.contains(key)) {
                QUrl url(chan.value(key));
                m_imageRequests.insert(url, id);
                sendRequest(url);
                return;
            }
        }
    }
}

QUrl DataProvider::getPlaylistUrl(QString id) {
    qDebug() << "DataProvider::getPlaylistUrl";

    if(m_channelData.contains(id)) {
        ChannelInfo chan = m_channelData.value(id);
        QStringList keys = QStringList() << "highestpls_aac"
                                         << "fastpls_aacp"
                                         << "fastpls_mp3"
                                         << "slowpls_aacp"
                                         << "slowpls__mp3";

        QStringList::const_iterator it;
        for(it=keys.constBegin(); it!=keys.constEnd(); ++it) {
            if(chan.contains((*it))) {
                return QUrl(chan.value(*it));
            }
        }
    }

    return QUrl();
}

void DataProvider::handleNetworkReply(QNetworkReply *reply) {
    qDebug() << "DataProvider::handleNetworkReply("
             << reply->url().toString() << ")";

    QByteArray data = reply->readAll();

    QUrl url = reply->url();
    if(url == m_channelsUrl) {
        parseChannelsXml(data);
    } else if(m_iconRequests.contains(url)) {
        QString id = m_iconRequests.value(url);
        QPixmap icon = QPixmap::fromImage(QImage::fromData(data));
        emit channelIconProvided(id, icon);
        m_iconRequests.remove(url);
    } else if(m_imageRequests.contains(url)) {
        QString id = m_imageRequests.value(url);
        QImage image = QImage::fromData(data);
        emit channelImageProvided(id, image);
        m_imageRequests.remove(url);
    }

    reply->deleteLater();
}

void DataProvider::parseChannelsXml(QByteArray data) {
    qDebug() << "DataProvider::parseChannelsXml()";

    QXmlStreamReader xml;
    xml.addData(data);

    QStringList channelListElements = QStringList() << "title" << "description"
                                                    << "genre" << "dj"
                                                    << "listeners"
                                                    << "lastplaying";
    ChannelData channelData;
    ChannelList channelList;
    while (!xml.atEnd()) {
        xml.readNext();
        if(xml.isStartElement()) {
            if(xml.name() == "channel") {
                ChannelInfo dataInfo, listInfo;
                QString id = xml.attributes().value("id").toString();
                listInfo.insert("id", id);
                while(!xml.atEnd() &&
                      !(xml.isEndElement() && xml.name() == "channel")) {
                    xml.readNext();
                    if(xml.isStartElement()) {
                        QString key = xml.name().toString().toLower();
                        if(xml.attributes().hasAttribute("format"))
                            key.append("_").append(xml.attributes().value("format").toString());
                        xml.readNext();
                        if(xml.isCharacters()) {
                            QString value = xml.text().toString();
                            if(channelListElements.contains(key)) {
                                listInfo.insert(key, value);
                            }
                            dataInfo.insert(key, value);
                        }
                    }
                }
                channelData.insert(id, dataInfo);
                channelList.append(listInfo);
            }
        }
    }
    m_channelData = channelData;
    emit channelListProvided(channelList);
}
