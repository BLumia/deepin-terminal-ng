#include "termwidget.h"
#include "termproperties.h"
#include "settings.h"
#include "define.h"

#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopServices>
#include <QDebug>
#include <QMenu>

#include <DDesktopServices>
#include <dinputdialog.h>

DWIDGET_USE_NAMESPACE

TermWidget::TermWidget(TermProperties properties, QWidget *parent)
    : QTermWidget(0, parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);

    setHistorySize(5000);

    // set shell program
    QString shell { getenv("SHELL") };
    setShellProgram(shell.isEmpty() ? "/bin/bash" : shell);
    setTerminalOpacity(Settings::instance()->opacity());
    setScrollBarPosition(QTermWidget::ScrollBarRight);
    setKeyboardCursorShape(static_cast<QTermWidget::KeyboardCursorShape>(Settings::instance()->cursorShape()));
    if (properties.contains(WorkingDir)) {
        setWorkingDirectory(properties[WorkingDir].toString());
    }
    if (properties.contains(Execute)) {
        QString args = properties[Execute].toString();
        qDebug() << args;
        QStringList argList = args.split(QRegExp(QStringLiteral("\\s+")), QString::SkipEmptyParts);
        if (argList.count() > 0) {
            setShellProgram(argList.at(0));
            argList.removeAt(0);
            if (argList.count()) {
                setArgs(argList);
            }
        }
    }

    // config
    setColorScheme(Settings::instance()->colorScheme());
    setTerminalFont(QFont(Settings::instance()->fontFamily(), Settings::instance()->fontPointSize()));
    setBlinkingCursor(Settings::instance()->cursorBlink());

    connect(this, &QTermWidget::urlActivated, this, [](const QUrl & url, bool fromContextMenu){
        if (QApplication::keyboardModifiers() & Qt::ControlModifier || fromContextMenu) {
            QDesktopServices::openUrl(url);
        }
    });

    connect(this, &QWidget::customContextMenuRequested, this, &TermWidget::customContextMenuCall);

    startShellProgram();
}

void TermWidget::customContextMenuCall(const QPoint &pos)
{
    QMenu menu;

    QList<QAction*> termActions = filterActions(pos);
    for (QAction *& action : termActions) {
        menu.addAction(action);
    }

    if (!menu.isEmpty()) {
        menu.addSeparator();
    }

    // add other actions here.
    if (!selectedText().isEmpty()) {
        menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy &Selection"), this, [this] {
            copyClipboard();
        });
    }

    menu.addAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this, [this] {
        pasteClipboard();
    });

    menu.addAction(QIcon::fromTheme("document-open-folder"), tr("&Open File Manager"), this, [this] {
        DDesktopServices::showFolder(QUrl::fromLocalFile(workingDirectory()));
    });

    menu.addSeparator();

    menu.addAction(QIcon::fromTheme("view-split-left-right"), tr("Split &Horizontally"), this, [this] {
        emit termRequestSplit(Qt::Horizontal);
    });

    menu.addAction(QIcon::fromTheme("view-split-top-bottom"), tr("Split &Vertically"), this, [this] {
        emit termRequestSplit(Qt::Vertical);
    });

    menu.addSeparator();

    bool isFullScreen = this->window()->windowState().testFlag(Qt::WindowFullScreen);
    if (isFullScreen) {
        menu.addAction(QIcon::fromTheme("view-restore-symbolic"), tr("Exit Full&screen"), this, [this] {
            window()->setWindowState(windowState() & ~Qt::WindowFullScreen);
        });
    } else {
        menu.addAction(QIcon::fromTheme("view-fullscreen-symbolic"), tr("Full&screen"), this, [this] {
            window()->setWindowState(windowState() | Qt::WindowFullScreen);
        });
    }

    menu.addAction(QIcon::fromTheme("edit-find"), tr("&Find"), this, [this] {
        // this is not a DTK look'n'feel search bar, but nah.
        toggleShowSearchBar();
    });

    menu.addSeparator();

    menu.addAction(QIcon::fromTheme("edit-rename"), tr("Rename Tab"), this, [this] {
        // TODO: Tab name as default text?
        bool ok;
        QString text = DInputDialog::getText(nullptr, tr("Rename Tab"), tr("Tab name:"),
                                             QLineEdit::Normal, QString(), &ok);
        if (ok) {
            emit termRequestRenameTab(text);
        }
    });

    menu.addSeparator();

    menu.addAction(QIcon::fromTheme("settings-configure"), tr("Settings"), this, [this] {
        emit termRequestOpenSettings();
    });

    // display the menu.
    menu.exec(mapToGlobal(pos));
}

TermWidgetWrapper::TermWidgetWrapper(TermProperties properties, QWidget *parent)
    : QWidget(parent)
    , m_term(new TermWidget(properties, this))
{
    initUI();

    connect(m_term, &QTermWidget::titleChanged, this, [this] { emit termTitleChanged(m_term->title()); });
    connect(m_term, &QTermWidget::finished, this, &TermWidgetWrapper::termClosed);
    // proxy signal:
    connect(m_term, &QTermWidget::termGetFocus, this, &TermWidgetWrapper::termGetFocus);
    connect(m_term, &TermWidget::termRequestSplit, this, &TermWidgetWrapper::termRequestSplit);
    connect(m_term, &TermWidget::termRequestRenameTab, this, &TermWidgetWrapper::termRequestRenameTab);
    connect(m_term, &TermWidget::termRequestOpenSettings, this, &TermWidgetWrapper::termRequestOpenSettings);
}

bool TermWidgetWrapper::isTitleChanged() const
{
    return m_term->isTitleChanged();
}

QString TermWidgetWrapper::title() const
{
    return m_term->title();
}

QFont TermWidgetWrapper::terminalFont() const
{
    return m_term->getTerminalFont();
}

void TermWidgetWrapper::setTerminalFont(QFont font)
{
    return m_term->setTerminalFont(font);
}

QString TermWidgetWrapper::workingDirectory()
{
    return m_term->workingDirectory();
}

void TermWidgetWrapper::sendText(const QString &text)
{
    return m_term->sendText(text);
}

void TermWidgetWrapper::setTerminalOpacity(qreal opacity)
{
    m_term->setTerminalOpacity(opacity);
}

void TermWidgetWrapper::setColorScheme(const QString &name)
{
    m_term->setColorScheme(name);
}

void TermWidgetWrapper::zoomIn()
{
    m_term->zoomIn();
}

void TermWidgetWrapper::zoomOut()
{
    m_term->zoomOut();
}

void TermWidgetWrapper::copyClipboard()
{
    m_term->copyClipboard();
}

void TermWidgetWrapper::pasteClipboard()
{
    m_term->pasteClipboard();
}

void TermWidgetWrapper::initUI()
{
    setFocusProxy(m_term);

    m_layout = new QVBoxLayout;
    setLayout(m_layout);

    m_layout->addWidget(m_term);
    m_layout->setContentsMargins(0, 0, 0, 0);
}
