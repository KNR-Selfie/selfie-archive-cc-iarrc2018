TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    include/urg.cpp \
    include/sharedmemory.cpp \
    include/process.cpp

HEADERS += \
    include/def.hpp \
    include/urg.hpp \
    include/sharedmemory.hpp \
    include/process.hpp

# OpenCV
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lopencv_stitching -lopencv_videostab -lopencv_superres -lopencv_hfs -lopencv_dnn_objdetect -lopencv_freetype -lopencv_img_hash -lopencv_face -lopencv_photo -lopencv_structured_light -lopencv_line_descriptor -lopencv_bioinspired -lopencv_tracking -lopencv_reg -lopencv_saliency -lopencv_rgbd -lopencv_optflow -lopencv_ximgproc -lopencv_aruco -lopencv_xfeatures2d -lopencv_shape -lopencv_ccalib -lopencv_dpm -lopencv_surface_matching -lopencv_stereo -lopencv_xphoto -lopencv_fuzzy -lopencv_bgsegm -lopencv_calib3d -lopencv_video -lopencv_datasets -lopencv_text -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_dnn -lopencv_ml -lopencv_flann -lopencv_plot -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect -lopencv_phase_unwrapping -lopencv_imgproc -lopencv_core

# URG
unix:!macx: LIBS += -L$$PWD/../../../../_Biblioteki/urg_library-1.2.2/src/ -lurg_c
INCLUDEPATH += $$PWD/../../../../_Biblioteki/urg_library-1.2.2/include/c
DEPENDPATH += $$PWD/../../../../_Biblioteki/urg_library-1.2.2/include/c
unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../_Biblioteki/urg_library-1.2.2/src/liburg_c.a
