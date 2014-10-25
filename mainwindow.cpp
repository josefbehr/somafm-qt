#include <QSettings>

#include "dataprovider.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    qDebug() << "MainWindow::MainWindow";
    QCoreApplication::setOrganizationName("somafm-qt");
    //QCoreApplication::setOrganizationDomain("somafm.com");
    QCoreApplication::setApplicationName("somafm-qt");
    readSettings();

    m_tabWidget = new QTabWidget;
    m_channelsView = new ChannelsView;

    m_tabWidget->addTab(m_channelsView, tr("Channels"));

    m_playerView = new PlayerView;

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(m_tabWidget);
    m_mainLayout->addWidget(m_playerView);

    m_mainWidget = new QFrame(this);
    m_mainWidget->setLayout(m_mainLayout);

    setCentralWidget(m_mainWidget);
    setWindowTitle(tr("SomaFM Radio Player"));

    m_dataProvider = new DataProvider;
    connect(m_dataProvider, SIGNAL(channelListProvided(ChannelList)),
            m_channelsView, SLOT(updateChannelList(ChannelList)));

    connect(m_channelsView, SIGNAL(requestChannelIcon(QString)),
            m_dataProvider, SLOT(provideChannelIcon(QString)));

    connect(m_dataProvider, SIGNAL(channelIconProvided(QString,QPixmap)),
            m_channelsView, SLOT(updateChannelIcon(QString,QPixmap)));

    connect(m_dataProvider, SIGNAL(channelImageProvided(QString,QImage)),
            m_playerView, SLOT(updateChannelImage(QString,QImage)));

    connect(m_channelsView, SIGNAL(channelActivated(QString)),
            this, SLOT(startPlayer(QString)));

    connect(m_playerView, SIGNAL(requestChannelImage(QString)),
            m_dataProvider, SLOT(provideChannelImage(QString)));

    m_dataProvider->provideChannelList();
}

void MainWindow::startPlayer(QString id) {
    qDebug() << "MainWindow::startPlayer";

    QUrl url = m_dataProvider->getPlaylistUrl(id);
    m_playerView->play(id, url);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    writeSettings();
    event->accept();
}

MainWindow::~MainWindow()
{
}

void MainWindow::writeSettings()
{
    QSettings settings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(400, 600)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}
