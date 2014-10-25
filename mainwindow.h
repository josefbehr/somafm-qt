#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QFrame>

#include "channelsview.h"
#include "dataprovider.h"
#include "playerview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void readSettings();
    void writeSettings();

private:
    QTabWidget *m_tabWidget;
    ChannelsView *m_channelsView;
    QVBoxLayout *m_mainLayout;
    QFrame *m_mainWidget;
    DataProvider *m_dataProvider;
    PlayerView *m_playerView;

public slots:
    void startPlayer(QString);
};

#endif // MAINWINDOW_H
