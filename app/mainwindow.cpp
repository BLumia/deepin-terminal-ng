#include "mainwindow.h"

#include "tabbar.h"
#include "settings.h"
#include "termwidgetpage.h"
#include "termproperties.h"
#include "shortcutmanager.h"
#include "mainwindowplugininterface.h"

#include "themepanelplugin.h"

#include <DTitlebar>
#include <DSettingsWidgetFactory>
#include <QVBoxLayout>
#include <QDebug>
#include <QApplication>
#include <QShortcut>
#include <QSettings>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(TermProperties properties, QWidget *parent) :
    DMainWindow(parent),
    m_menu(new QMenu),
    m_tabbar(new TabBar),
    m_centralWidget(new QWidget(this)),
    m_centralLayout(new QVBoxLayout(m_centralWidget)),
    m_termStackWidget(new QStackedWidget),
    m_titlebarStyleSheet(titlebar()->styleSheet()),
    m_shortcutManager(new ShortcutManager(this))
{
    setAttribute(Qt::WA_TranslucentBackground);

    setMinimumSize(450, 250);
    setCentralWidget(m_centralWidget);
    setWindowIcon(QIcon::fromTheme("deepin-terminal"));

    // Init layout
    m_centralLayout->setContentsMargins(0, 0, 0, 0);
    m_centralLayout->setSpacing(0);

    m_centralLayout->addWidget(m_termStackWidget);

    initConnections();
    initShortcuts();

    // Plugin may need centralWidget() to work so make sure initPlugin() is after setCentralWidget()
    // Other place (eg. create titlebar menu) will call plugin method so we should create plugins before init other parts.
    initPlugins();

    initWindow();
    initTitleBar();
    addTab(properties);
}

MainWindow::~MainWindow()
{
    //
}

void MainWindow::addTab(TermProperties properties, bool activeTab)
{
    TermWidgetPage *termPage = new TermWidgetPage(properties);
    setNewTermPage(termPage, activeTab);
    int index = m_tabbar->addTab(termPage->identifier(), "New Terminal Tab");
    if (activeTab) {
        m_tabbar->setCurrentIndex(index);
    }
    connect(termPage, &TermWidgetPage::termTitleChanged, this, &MainWindow::onTermTitleChanged);
    connect(termPage, &TermWidgetPage::tabTitleChanged, this, &MainWindow::onTabTitleChanged);
    connect(termPage, &TermWidgetPage::termRequestOpenSettings, this, &MainWindow::showSettingDialog);
    connect(termPage, &TermWidgetPage::lastTermClosed, this, &MainWindow::closeTab);
}

void MainWindow::closeTab(const QString &identifier)
{
    for (int i = 0, count = m_termStackWidget->count(); i < count; i++) {
        TermWidgetPage *tabPage = qobject_cast<TermWidgetPage*>(m_termStackWidget->widget(i));
        if (tabPage && tabPage->identifier() == identifier) {
            m_termStackWidget->removeWidget(tabPage);
            tabPage->deleteLater();
            m_tabbar->removeTab(identifier);
            break;
        }
    }

    if (m_tabbar->count() == 0) {
        qApp->quit();
    }
}

void MainWindow::focusTab(const QString &identifier)
{
    for (int i = 0, count = m_termStackWidget->count(); i < count; i++) {
        TermWidgetPage *tabPage = qobject_cast<TermWidgetPage*>(m_termStackWidget->widget(i));
        if (tabPage && tabPage->identifier() == identifier) {
            m_termStackWidget->setCurrentWidget(tabPage);
            tabPage->focusCurrentTerm();
            break;
        }
    }
}

TermWidgetPage *MainWindow::currentTab()
{
    return qobject_cast<TermWidgetPage *>(m_termStackWidget->currentWidget());
}

void MainWindow::forAllTabPage(const std::function<void(TermWidgetPage*)> &func)
{
    for (int i = 0, count = m_termStackWidget->count(); i < count; i++) {
        TermWidgetPage *tabPage = qobject_cast<TermWidgetPage*>(m_termStackWidget->widget(i));
        if (tabPage) {
            func(tabPage);
        }
    }
}

void MainWindow::setTitleBarBackgroundColor(QString color)
{
    Q_UNUSED(color);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings("blumia", "dterm");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    DMainWindow::closeEvent(event);
}

void MainWindow::onTermTitleChanged(QString title)
{
    TermWidgetPage *tabPage = qobject_cast<TermWidgetPage*>(sender());
    const bool customName = tabPage->property("TAB_CUSTOM_NAME_PROPERTY").toBool();
    if (!customName) {
        m_tabbar->setTabText(tabPage->identifier(), title);
    }
}

void MainWindow::onTabTitleChanged(QString title)
{
    TermWidgetPage *tabPage = qobject_cast<TermWidgetPage*>(sender());
    m_tabbar->setTabText(tabPage->identifier(), title);
}

void MainWindow::onBuiltinShortcutTriggered()
{
    QObject * sender = QObject::sender();
    if (!sender->property("IS_BUILTIN_SHORTCUT").toBool()) {
        return;
    }

    TermWidgetPage *page = currentTab();
    QString name = sender->objectName();

    if (name == "shortcuts.terminal.copy") {
        if (page) page->copyClipboard();
    } else if (name == "shortcuts.terminal.find") {
        if (page) page->toggleShowSearchBar();
    } else if (name == "shortcuts.terminal.paste") {
        if (page) page->pasteClipboard();
    } else if (name == "shortcuts.terminal.zoom_in") {
        if (page) page->zoomInCurrentTierminal();
    } else if (name == "shortcuts.terminal.zoom_out") {
        if (page) page->zoomOutCurrentTerminal();
    } else if (name == "shortcuts.workspace.focus_nav_down") {
        if (page) page->focusNavigation(Qt::BottomEdge);
    } else if (name == "shortcuts.workspace.focus_nav_left") {
        if (page) page->focusNavigation(Qt::LeftEdge);
    } else if (name == "shortcuts.workspace.focus_nav_right") {
        if (page) page->focusNavigation(Qt::RightEdge);
    } else if (name == "shortcuts.workspace.focus_nav_up") {
        if (page) page->focusNavigation(Qt::TopEdge);
    } else if (name == "shortcuts.workspace.horizontal_split") {
        if (page) page->split(Qt::Horizontal);
    } else if (name == "shortcuts.workspace.new_tab") {
        this->addTab(currentTab()->createCurrentTerminalProperties(), true);
    } else if (name == "shortcuts.workspace.vertical_split") {
        if (page) page->split(Qt::Vertical);
    }
}

void MainWindow::initPlugins()
{
    // Todo: real plugin loader and plugin support.
    ThemePanelPlugin *testPlugin = new ThemePanelPlugin(this);
    testPlugin->initPlugin(this);

    m_plugins.append(testPlugin);
}

void MainWindow::initWindow()
{
    QSettings settings("blumia", "dterm");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    setEnableBlurWindow(Settings::instance()->backgroundBlur());
}

void MainWindow::initShortcuts()
{
    m_shortcutManager->initShortcuts();

    for (const QAction * action : this->actions()) {
        connect(action, &QAction::triggered, this, &MainWindow::onBuiltinShortcutTriggered);
    }
}

void MainWindow::initConnections()
{
    connect(Settings::instance(), &Settings::opacityChanged, this, [this](qreal opacity) {
        forAllTabPage([opacity](TermWidgetPage * tabPage) {
            tabPage->setTerminalOpacity(opacity);
        });
    });

    connect(Settings::instance(), &Settings::fontFamilyChanged, this, [this](QString fontFamily) {
        forAllTabPage([fontFamily](TermWidgetPage * tabPage) {
            tabPage->setFontFamily(fontFamily);
        });
    });

    connect(Settings::instance(), &Settings::fontPointSizeChanged, this, [this](int pointSize) {
        forAllTabPage([pointSize](TermWidgetPage * tabPage) {
            tabPage->setFontPointSize(pointSize);
        });
    });

    connect(Settings::instance(), &Settings::backgroundBlurChanged, this, [this](bool enabled) {
        setEnableBlurWindow(enabled);
    });
}

void MainWindow::initTitleBar()
{
    for (MainWindowPluginInterface* plugin : m_plugins) {
        QAction * pluginMenu = plugin->titlebarMenu(this);
        if (pluginMenu) {
            m_menu->addAction(pluginMenu);
        }
    }

    QAction *settingAction(new QAction(tr("&Settings"), this));
    m_menu->addAction(settingAction);

    titlebar()->setIcon(QIcon::fromTheme("deepin-terminal", QIcon::fromTheme("utilities-terminal")));
    titlebar()->setCustomWidget(m_tabbar);
    titlebar()->setAutoHideOnFullscreen(true);
    titlebar()->setMenu(m_menu);

    connect(m_tabbar, &DTabBar::tabCloseRequested, this, [this](int index){
        closeTab(m_tabbar->identifier(index));
    }, Qt::QueuedConnection);

    connect(m_tabbar, &DTabBar::tabAddRequested, this, [this](){
        addTab(currentTab()->createCurrentTerminalProperties(), true);
    }, Qt::QueuedConnection);

    connect(m_tabbar, &DTabBar::currentChanged, this, [this](int index){
        focusTab(m_tabbar->identifier(index));
    }, Qt::QueuedConnection);

    connect(settingAction, &QAction::triggered, this, &MainWindow::showSettingDialog);
}

void MainWindow::setNewTermPage(TermWidgetPage *termPage, bool activePage)
{
    m_termStackWidget->addWidget(termPage);
    if (activePage) {
        m_termStackWidget->setCurrentWidget(termPage);
    }
}

void MainWindow::showSettingDialog()
{
    QScopedPointer<DSettingsDialog> dialog(new DSettingsDialog(this));
    dialog->widgetFactory()->registerWidget("fontcombobox", Settings::createQFontComboBoxHandle);
    dialog->updateSettings(Settings::instance()->settings);
    dialog->exec();
}
