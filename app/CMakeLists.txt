cmake_minimum_required(VERSION 3.13.0)

# Populate a CMake variable with the sources
# TODO: portable headers?
set (DTNG_CPP_FILES
    main.cpp
    mainwindow.cpp
    tabbar.cpp
    termwidget.cpp
    termwidgetpage.cpp
    rightpanel.cpp
    themepanel.cpp
    themelistview.cpp
    themelistmodel.cpp
    themeitemdelegate.cpp
    termproperties.cpp
    settings.cpp
    shortcutmanager.cpp
    themepanelplugin.cpp
)

set (DTNG_HEADER_FILES
    define.h
    mainwindow.h
    tabbar.h
    termwidget.h
    termwidgetpage.h
    rightpanel.h
    themepanel.h
    themelistview.h
    themelistmodel.h
    themeitemdelegate.h
    termproperties.h
    settings.h
    shortcutmanager.h
    themepanelplugin.h
)

set (DTNG_PUBLIC_HEADER_FILES
    mainwindowplugininterface.h
)

target_sources(${EXE_NAME}
PUBLIC
    ${DTNG_PUBLIC_HEADER_FILES}
PRIVATE
    ${DTNG_HEADER_FILES}
    ${DTNG_CPP_FILES}
)
