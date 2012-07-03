QT += gui core

TARGET = bayes-gui
TEMPLATE = app

INCLUDEPATH += $(SEXPR)
LIBS += -L$(SEXPR) -lsexp

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    network.cpp \
    node.cpp \
    networkeditor.cpp \
    engine.cpp \
    networkdock.cpp \
    nodedock.cpp \
    networkeditortabs.cpp \
    graphicsedge.cpp \
    graphicsnode.cpp \
    valuemodel.cpp \
    edgemodel.cpp \
    probabilitymodel.cpp \
    settingsdialog.cpp

HEADERS += \
    mainwindow.h \
    settings.h \
    network.h \
    node.h \
    networkeditor.h \
    engine.h \
    networkdock.h \
    nodedock.h \
    networkeditortabs.h \
    graphicsedge.h \
    graphicsnode.h \
    valuemodel.h \
    edgemodel.h \
    probabilitymodel.h \
    settingsdialog.h
