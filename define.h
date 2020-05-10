#ifndef DTNG_DEFINE_H
#define DTNG_DEFINE_H

#include <QtGlobal>

/*
   QTERMWIDGET_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QTERMWIDGET_VERSION >= QT_VERSION_CHECK(0, 7, 1))

   Note:
   upstream defined this variable in https://github.com/lxqt/qtermwidget/commit/b025013706483daaa073ff3dc7a137f5f04258d5
   but that commit is only available in version >= 0.15.0, so just leave it here now...
   Current debian stable: buster qtermwidget 0.14.1-1
*/
#ifndef QTERMWIDGET_VERSION
#define QTERMWIDGET_VERSION      QT_VERSION_CHECK(QTERMWIDGET_VERSION_MAJOR, QTERMWIDGET_VERSION_MINOR, QTERMWIDGET_VERSION_PATCH)
#endif

#endif // DTNG_DEFINE_H
