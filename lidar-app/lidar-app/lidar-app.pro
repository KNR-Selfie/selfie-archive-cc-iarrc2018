TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp \
    include/urg.cpp

HEADERS += \
    include/def.hpp \
    include/urg.hpp

# OpenCV
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_xfeatures2d -lopencv_shape -lopencv_img_hash -lopencv_dpm -lopencv_optflow -lopencv_rgbd -lopencv_xphoto -lopencv_ximgproc -lopencv_line_descriptor -lopencv_fuzzy -lopencv_saliency -lopencv_stereo -lopencv_reg -lopencv_bgsegm -lopencv_freetype -lopencv_structured_light -lopencv_phase_unwrapping -lopencv_bioinspired -lopencv_aruco -lopencv_ccalib -lopencv_calib3d -lopencv_face -lopencv_tracking -lopencv_datasets -lopencv_text -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_dnn -lopencv_video -lopencv_plot -lopencv_photo -lopencv_ml -lopencv_xobjdetect -lopencv_imgcodecs -lopencv_objdetect -lopencv_imgproc -lopencv_surface_matching -lopencv_flann -lopencv_core

# URG
unix:!macx: LIBS += -L$$PWD/../../../../_Biblioteki/urg_library-1.2.2/src/ -lurg_c
INCLUDEPATH += $$PWD/../../../../_Biblioteki/urg_library-1.2.2/include/c
DEPENDPATH += $$PWD/../../../../_Biblioteki/urg_library-1.2.2/include/c
unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../_Biblioteki/urg_library-1.2.2/src/liburg_c.a
