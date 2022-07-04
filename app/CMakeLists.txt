cmake_minimum_required(VERSION 3.13.0)

# Program
set (EXE_NAME dterm)

# Find the QtWidgets library
find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets LinguistTools)
find_package(Dtk REQUIRED Core Widget Tools)
find_package(QTermWidget5 REQUIRED)

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

set (DTNG_QRC_FILES
    ../resources.qrc
)

# Translation
file (GLOB DTNG_TS_FILES ${PROJECT_SOURCE_DIR}/translations/*.ts)
set (DTNG_CPP_FILES_FOR_I18N ${DTNG_CPP_FILES})

dtk_create_i18n_from_json(DTNG_SETTINGS_I18N_FILES ${PROJECT_SOURCE_DIR}/default-config.json settings_translation.cpp)
list (APPEND DTNG_CPP_FILES_FOR_I18N ${DTNG_SETTINGS_I18N_FILES})
qt5_create_translation(DTNG_QM_FILES ${DTNG_CPP_FILES_FOR_I18N} ${DTNG_TS_FILES})
message(INFO ${DTNG_CPP_FILES} )

#target_sources(${EXE_NAME}
#PUBLIC
#    ${DTNG_PUBLIC_HEADER_FILES}
#PRIVATE
#    ${DTNG_HEADER_FILES}
#    ${DTNG_CPP_FILES}
#    ${DTNG_QRC_FILES}
#)
add_executable(${EXE_NAME}
    ${DTNG_PUBLIC_HEADER_FILES}
    ${DTNG_HEADER_FILES}
    ${DTNG_CPP_FILES}
    ${DTNG_QRC_FILES}
    ${DTNG_QM_FILES}
)

target_include_directories(${EXE_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${QTERMWIDGET_INCLUDE_DIRS}
    ./
)

target_link_libraries (${EXE_NAME} Qt5::Widgets ${DtkWidget_LIBRARIES} qtermwidget5)

# Compile definitions for QTermWidget
# So we can use QT_VERSION_CHECK
# Note: upstream add that define in https://github.com/lxqt/qtermwidget/commit/b025013706483daaa073ff3dc7a137f5f04258d5
#       but that commit is only available in version >= 0.15.0, so just leave it here now...
target_compile_definitions(${EXE_NAME} PRIVATE
    QTERMWIDGET_VERSION_MAJOR=${QTermWidget5_VERSION_MAJOR}
    QTERMWIDGET_VERSION_MINOR=${QTermWidget5_VERSION_MINOR}
    QTERMWIDGET_VERSION_PATCH=${QTermWidget5_VERSION_PATCH}
)

install(TARGETS ${EXE_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES ${DTNG_QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/dterm/translations)
