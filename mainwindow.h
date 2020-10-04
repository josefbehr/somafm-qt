#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QFrame>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QtWebEngineWidgets>

#include "channelsview.h"
#include "dataprovider.h"
#include "playerview.h"
#include "settingsview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);
    void createTrayIcon();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readSettings();
    void writeSettings();

private:
    QLabel *m_topView;
    QTabWidget *m_tabWidget;
    ChannelsView *m_channelsView;
    QVBoxLayout *m_mainLayout;
    QFrame *m_mainWidget;
    DataProvider *m_dataProvider;
    PlayerView *m_playerView;
    QSystemTrayIcon *m_trayIcon;
    SettingsView *m_settingsView;
    QWebEngineView *m_newsView;
    QWebEngineView *m_historyView;

public slots:
    void startPlayer(QString);
    void trayIconActivated(QSystemTrayIcon::ActivationReason);
    void showTrayIcon(bool);
    void updateTitle(QString);
};

#endif // MAINWINDOW_H
