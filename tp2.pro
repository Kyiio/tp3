INCLUDEPATH += $$PWD
SOURCES += $$PWD/openglwindow.cpp \
    gamewindow.cpp \
    Camera.cpp \
    gamewindowclient.cpp \
    gamewindowserveur.cpp \
    mythread.cpp
HEADERS += $$PWD/openglwindow.h \
    Camera.h \
    gamewindow.h \
    gamewindowclient.h \
    gamewindowserveur.h \
    mythread.h

SOURCES += \
    main.cpp

target.path = .
INSTALLS += target

RESOURCES += \
    gestionnaire.qrc

QT += network
