#include "mainwindow.h"

#include "titlebar.h"
#include "tabbar.h"
#include "termwidgetpage.h"

#include <QVBoxLayout>
#include <DTitlebar>
#include <QDebug>
#include <QApplication>
#include <DBlurEffectWidget>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent) :
    DMainWindow(parent),
    m_tabbar(new TabBar),
    m_centralWidget(new DBlurEffectWidget(parent)),
    m_centralLayout(new QVBoxLayout(m_centralWidget)),
    m_termStackWidget(new QStackedWidget)
{
    setMinimumSize(450, 250);
    setCentralWidget(m_centralWidget);
    setWindowIcon(QIcon::fromTheme("deepin-terminal"));

    // Init layout
    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    m_centralLayout->addWidget(m_termStackWidget);

    DBlurEffectWidget *cwb = (DBlurEffectWidget*)m_centralWidget;
    cwb->setBlendMode(DBlurEffectWidget::BlendMode::BehindWindowBlend);
    cwb->setRadius(16);

    initTitleBar();

    addTab();
}

MainWindow::~MainWindow()
{
    //
}

void MainWindow::addTab(bool activeTab)
{
    TermWidgetPage *termPage = new TermWidgetPage();
    setNewTermPage(termPage, activeTab);
    int index = m_tabbar->addTab(termPage->identifier(), "New Terminal Tab");
    if (activeTab) {
        m_tabbar->setCurrentIndex(index);
    }
    connect(termPage, &TermWidgetPage::termTitleChanged, this, &MainWindow::onTermTitleChanged);
    connect(termPage, &TermWidgetPage::tabTitleChanged, this, &MainWindow::onTabTitleChanged);
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

void MainWindow::initTitleBar()
{
    TitleBar *titleBar = new TitleBar;
    titleBar->setTabBar(m_tabbar);
    titlebar()->setCustomWidget(titleBar, Qt::AlignVCenter, false);
    titlebar()->setAutoHideOnFullscreen(true);

    connect(m_tabbar, &DTabBar::tabCloseRequested, this, [this](int index){
        closeTab(m_tabbar->identifier(index));
    }, Qt::QueuedConnection);

    connect(m_tabbar, &DTabBar::tabAddRequested, this, [this](){
        addTab(true);
    }, Qt::QueuedConnection);

    connect(m_tabbar, &DTabBar::currentChanged, this, [this](int index){
        focusTab(m_tabbar->identifier(index));
    }, Qt::QueuedConnection);
}

void MainWindow::setNewTermPage(TermWidgetPage *termPage, bool activePage)
{
    m_termStackWidget->addWidget(termPage);
    if (activePage) {
        m_termStackWidget->setCurrentWidget(termPage);
    }
}
