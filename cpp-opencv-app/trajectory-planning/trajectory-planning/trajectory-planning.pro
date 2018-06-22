TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11


HEADERS += \
    include/tangent.hpp \
    include/spline.hpp \
    include/spline.h \
    include/sliders.h \
    include/sharedmemory.hpp \
    include/vvector.h \
    include/interpolation.h \
    include/usb.hpp \
    main.h \
    include/matrix.h \
    include/optimization.h \
    include/file.h


SOURCES += main.cpp \
    include/tangent.cpp \
    include/spline.cpp  \
    include/sharedmemory.cpp \
    include/interpolation.cpp \
    include/usb.cpp \
    include/sliders.cpp \
    include/optimization.cpp \
    include/file.cpp


INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_stitching -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_dnn -lopencv_video  -lopencv_photo -lopencv_ml  -lopencv_imgcodecs -lopencv_objdetect -lopencv_imgproc  -lopencv_flann -lopencv_core
LIBS += -pthread



