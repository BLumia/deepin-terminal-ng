#ifndef SETTINGS_H
#define SETTINGS_H

#include <DSettingsDialog>
#include <qsettingbackend.h>

DCORE_USE_NAMESPACE

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings *instance();

    void initConnection();

    qreal opacity() const;
    QString colorScheme() const;
    QString fontFamily() const;
    int fontPointSize() const;
    int cursorShape() const;
    bool cursorBlink() const;
    bool backgroundBlur() const;

    void setColorScheme(const QString &name);

    QList<QAction *> createShortcutActions();

    static QPair<QWidget *, QWidget *> createQFontComboBoxHandle(QObject *obj);

    DSettings *settings;

signals:
    void settingValueChanged(const QString &key, const QVariant &value);

    void opacityChanged(qreal opacity);
    void fontFamilyChanged(QString fontFamily);
    void fontPointSizeChanged(int pointSize);
    void cursorShapeChanged(int shape);
    void cursorBlinkChanged(bool blink);
    void backgroundBlurChanged(bool enabled);

private:
    Settings();

    static Settings *m_settings_instance;

    Dtk::Core::QSettingBackend *m_backend;
    QString m_configPath;
};

#endif // SETTINGS_H
