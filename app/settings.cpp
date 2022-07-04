#include "settings.h"

#include <QDebug>
#include <QAction>
#include <QApplication>
#include <QStandardPaths>
#include <DSettingsOption>
#include <QFontComboBox>
#include <DSettingsWidgetFactory>

#include "shortcutmanager.h"

DWIDGET_USE_NAMESPACE

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

    QPointer<DSettingsOption> fontFamily = settings->option("basic.interface.font_family");
    connect(fontFamily, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
        emit fontFamilyChanged(value.toString());
    });

    QPointer<DSettingsOption> fontPointSize = settings->option("basic.interface.font_point_size");
    connect(fontPointSize, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
        emit fontPointSizeChanged(value.toInt());
    });

    QPointer<DSettingsOption> cursorShape = settings->option("advanced.cursor.shape");
    connect(cursorShape, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
        emit cursorShapeChanged(value.toInt());
    });

    QPointer<DSettingsOption> cursorBlink = settings->option("advanced.cursor.blink");
    connect(cursorBlink, &Dtk::Core::DSettingsOption::valueChanged, this, [=] (QVariant value) {
        emit cursorBlinkChanged(value.toBool());
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

QString Settings::colorScheme() const
{
    return settings->option("basic.interface.theme")->value().toString();
}

QString Settings::fontFamily() const
{
    return settings->option("basic.interface.font_family")->value().toString();
}

int Settings::fontPointSize() const
{
    return settings->option("basic.interface.font_point_size")->value().toInt();
}

int Settings::cursorShape() const
{
    return settings->option("advanced.cursor.shape")->value().toInt();
}

bool Settings::cursorBlink() const
{
    return settings->option("advanced.cursor.blink")->value().toBool();
}

bool Settings::backgroundBlur() const
{
    return settings->option("advanced.window.blur_background")->value().toBool();
}

void Settings::setColorScheme(const QString &name)
{
    return settings->option("basic.interface.theme")->setValue(name);
}

QList<QAction *> Settings::createShortcutActions()
{
    QList<QAction *> actionList;

    const auto options = settings->options();
    for (const auto & option : options) {
        if (option.data()->key().startsWith("shortcuts.") && option.data()->viewType() == "shortcut") {
            actionList << ShortcutManager::createBuiltinShortcutAction(option.data()->key(), option.data()->value().toString());
        }
    }

    return actionList;
}

QPair<QWidget *, QWidget *> Settings::createQFontComboBoxHandle(QObject *obj)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(obj);

    QFontComboBox *comboBox = new QFontComboBox;
    comboBox->setFontFilters(QFontComboBox::MonospacedFonts);

    if (!option->value().toString().isEmpty()) {
        comboBox->setCurrentFont(QFont(option->value().toString()));
    }

    QPair<QWidget *, QWidget *> optionWidget = DSettingsWidgetFactory::createStandardItem(QByteArray(), option, comboBox);

    option->connect(comboBox, &QFontComboBox::currentFontChanged, option, [ = ](const QFont & font) {
        option->setValue(font.family());
    });

    return optionWidget;
}
