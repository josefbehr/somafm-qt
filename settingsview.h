#ifndef SETTINGSVIEW_H
#define SETTINGSVIEW_H

#include <QtWidgets>
#include <QWidget>

class SettingsView : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsView(QWidget *parent = 0);
    ~SettingsView();

private:
    QCheckBox *m_showTrayBox;

signals:
    void showTrayIcon(bool);

public slots:
    void applySettings();
};

#endif // SETTINGSVIEW_H
