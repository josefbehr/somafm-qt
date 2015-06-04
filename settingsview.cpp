#include <QSettings>

#include "settingsview.h"

SettingsView::SettingsView(QWidget *parent) : QWidget(parent)
{
    qDebug() << "SettingsView::SettingsView";

    QSettings settings;
    QGroupBox *trayGroup = new QGroupBox(tr("Tray Icon"));

    m_showTrayBox = new QCheckBox(tr("Show icon in system tray"), this);
    m_showTrayBox->setChecked(settings.value("showTrayIcon", true).toBool());
    connect(m_showTrayBox, SIGNAL(toggled(bool)),
            this, SIGNAL(showTrayIcon(bool)));

    QVBoxLayout *trayLayout = new QVBoxLayout;
    trayLayout->addWidget(m_showTrayBox);
    trayLayout->setAlignment(Qt::AlignCenter);
    trayGroup->setLayout(trayLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(trayGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void SettingsView::applySettings() {
    qDebug() << "SettingsView::applySettings()";

    emit showTrayIcon(m_showTrayBox->isChecked());
}

SettingsView::~SettingsView() {
    QSettings settings;
    settings.setValue("showTrayIcon", m_showTrayBox->isChecked());
}
