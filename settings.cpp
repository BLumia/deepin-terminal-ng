#include "settings.h"

#include <QApplication>
#include <QStandardPaths>
#include <DSettingsOption>

Settings *Settings::m_settings_instance = nullptr;

Settings::Settings()
    : QObject(qApp)
{
    m_configPath = QString("%1/%2/%3/config.conf")
        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
        .arg(qApp->organizationName())
        .arg(qApp->applicationName());

    m_backend = new QSettingBackend(m_configPath);

    settings = DSettings::fromJsonFile(":/config/default-config.json");
    settings->setBackend(m_backend);

    initConnection();
}

Settings *Settings::instance()
{
    if (!m_settings_instance) {
        m_settings_instance = new Settings;
    }

    return m_settings_instance;
}

void Settings::initConnection()
{
    connect(settings, &Dtk::Core::DSettings::valueChanged, this, [=] (const QString &key, const QVariant &value) {
        emit settingValueChanged(key, value);
    });

    QPointer<DSettingsOption> opacity = settings->option("basic.interface.opacity");
    connect(opacity, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
        emit opacityChanged(value.toInt() / 100.0);
    });

    QPointer<DSettingsOption> backgroundBlur = settings->option("advanced.window.blur_background");
    connect(backgroundBlur, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
        emit backgroundBlurChanged(value.toBool());
    });
}

qreal Settings::opacity() const
{
    return settings->option("basic.interface.opacity")->value().toInt() / 100.0;
}

bool Settings::backgroundBlur() const
{
    return settings->option("advanced.window.blur_background")->value().toBool();
}
