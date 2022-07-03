cmake_minimum_required(VERSION 3.13.0)

project(dtng)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")
set(QT_MINIMUM_VERSION "5.7.1")

# Program
set (EXE_NAME dterm)

# Find the QtWidgets library
find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets LinguistTools)
find_package(Dtk REQUIRED Core Widget Tools)
find_package(QTermWidget5 REQUIRED)

add_executable(${EXE_NAME} "")

add_subdirectory(app)

set (DTNG_QRC_FILES
    resources.qrc
)

# Translation
file (GLOB DTNG_TS_FILES translations/*.ts)
set (DTNG_CPP_FILES_FOR_I18N ${DTNG_CPP_FILES})

dtk_create_i18n_from_json(DTNG_SETTINGS_I18N_FILES default-config.json settings_translation.cpp)
list (APPEND DTNG_CPP_FILES_FOR_I18N ${DTNG_SETTINGS_I18N_FILES})
qt5_create_translation(DTNG_QM_FILES ${DTNG_CPP_FILES_FOR_I18N} ${DTNG_TS_FILES})

target_sources(${EXE_NAME}
PRIVATE
    ${DTNG_QRC_FILES}
    ${DTNG_QM_FILES}
)

target_include_directories(${EXE_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${QTERMWIDGET_INCLUDE_DIRS}
    ./app
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

# Install settings

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

install(TARGETS ${EXE_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES ${DTNG_QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/dterm/translations)
install(FILES dist/dterm.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications/)
