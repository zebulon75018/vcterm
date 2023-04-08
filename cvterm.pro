ADS_OUT_ROOT = Qt-Advanced-Docking-System

CONFIG+=fontAwesomeFree


OBJECTS_DIR = .obj
MOC_DIR     = .moc

QT += core gui widgets webkitwidgets network

include(QtAwesome/QtAwesome/QtAwesome.pri )
include(QSingleInstance/de_skycoder42_qsingleinstance.pri)

TARGET = vcterm
#DESTDIR = $${ADS_OUT_ROOT}/lib
TEMPLATE = app
CONFIG += c++14
CONFIG += debug_and_release
adsBuildStatic {
    DEFINES += ADS_STATIC
}

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        MainWindow.cpp \
        parametermanager.cpp

HEADERS += \
        MainWindow.h \
        parametermanager.h

#FORMS += \
#        MainWindow.ui
        

LIBS += -L$${ADS_OUT_ROOT}/lib -L./qtterminalwidget/ -L./QtAwesome/QtAwesome/
LIBS +=  -lqtterminalwidget -lX11

# Dependency: AdvancedDockingSystem (shared)
CONFIG(debug, debug|release){
    win32 {
        LIBS += -lqtadvanceddockingd
    }
    else:mac {
        LIBS += -lqtadvanceddocking_debug
    }
    else {
        LIBS += -lqtadvanceddocking
    }
}
else{
    LIBS += -lqtadvanceddocking
}

LIBS += -lQtAwesome

INCLUDEPATH += Qt-Advanced-Docking-System/src QSingleInstance
DEPENDPATH += Qt-Advanced-Docking-System/src    

