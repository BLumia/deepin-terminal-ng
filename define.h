#ifndef DTNG_DEFINE_H
#define DTNG_DEFINE_H

#include <QtGlobal>

/*
   QTERMWIDGET_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QTERMWIDGET_VERSION >= QT_VERSION_CHECK(0, 7, 1))
*/
#define QTERMWIDGET_VERSION      QT_VERSION_CHECK(QTERMWIDGET_VERSION_MAJOR, QTERMWIDGET_VERSION_MINOR, QTERMWIDGET_VERSION_PATCH)

enum NavigationDirection {
    Left,
    Right,
    Up,
    Down
};

#endif // DTNG_DEFINE_H
