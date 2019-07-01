#ifndef SETTINGS_H
#define SETTINGS_H

#include <DSettingsDialog>
#include <qsettingbackend.h>

DCORE_USE_NAMESPACE

class Settings : public QObject
{
    Q_OBJECT
public:
    Settings(QWidget *parent = nullptr);

    void initConnection();

    int opacity() const; // from 0 to 100, since DSettings only support int for slider.
    bool backgroundBlur() const;

    DSettings *settings;

signals:
    void settingValueChanged(const QString &key, const QVariant &value);

    void opacityChanged(int opacityRange);
    void backgroundBlurChanged(bool enabled);

private:
    Dtk::Core::QSettingBackend *m_backend;
    QString m_configPath;
};

#endif // SETTINGS_H
