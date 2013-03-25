#-------------------------------------------------
#
# Project created by QtCreator 2013-03-15T21:35:39
#
#-------------------------------------------------

QT       += core gui opengl
ICON = img/play_icon.png

#for c++ 11 support
QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lib309
TEMPLATE = lib
CONFIG += staticlib link_pkgconfig

PKGCONFIG += opencv
LIBS += -lv4l2 -lX11
#LIBS += -lwavelet2d -Lwavelet2s

INCLUDEPATH += /usr/local/lib/

SOURCES += \
    VideoStreamProcessor.cpp \
    VideoPlayControlsWidget.cpp \
    TimeDisplayerWidget.cpp \
    TimeControlWidget.cpp \
    Target.cpp \
    SwaperWidget.cpp \
    RobustMatcher.cpp \
    ProcessingChoiceWidget.cpp \
    PlayPauseButtonWidget.cpp \
    OpenCV_SynchronizedGLWidget.cpp \
    OpenCV_GLWidget.cpp \
    NetworkStreamInputDialog.cpp \
    Matcher_TargetMatcherV2_BinaryMask.cpp \
    Matcher_TargetMatcherV1.cpp \
    Mat2QImage.cpp \
    InputChoiceWidget.cpp \
    ImageDisplayerWidgetImpl3.cpp \
    ImageDisplayerWidgetImpl2.cpp \
    ImageDisplayerWidgetImpl.cpp \
    GLWidgetRenderingThread.cpp \
    CustomTextInput.cpp \
    CustomComboBox.cpp \
    CustomButton.cpp \
    Algorithm_OomsChallenge.cpp \
    Algorithm_OomsAlgorithmTest.cpp \
    Algorithm_NoProcessing.cpp \
    Algorithm_FeatureProjection.cpp \
    Algorithm_FaceDetector_Surf.cpp \
    Algorithm_FaceDetector.cpp \
    Algorithm_EyeFaceDetector.cpp \
    Algorithm_Condensation.cpp \
    Algorithm_ColorDetection.cpp \
    Algorithm_BinaryMaskWithOriginalFrame.cpp \
    BasicWindow.cpp \
    CondensationParameters.cpp

HEADERS += \
    VideoStreamReader.h \
    VideoStreamProcessor.h \
    VideoStreamInfo.h \
    VideoPlayControlsWidget.h \
    VideoCaptureDeviceDetectorForLinux.h \
    VideoCaptureDeviceDetector.h \
    TimeDisplayerWidget.h \
    TimeControlWidget.h \
    Target_FeatureProjectionTarget.h \
    Target_CondensationTargetV1.h \
    Target_CondensationTarget.h \
    Target.h \
    SwaperWidget.h \
    StreamImpl.h \
    StreamDisplayer.h \
    Stream.h \
    RobustMatcher.h \
    ProcessingChoiceWidget.h \
    PlayPauseButtonWidget.h \
    OpenCV_SynchronizedGLWidget.h \
    OpenCV_GLWidget.h \
    NetworkStreamInputDialog.h \
    Matcher_TargetMatcherV2_BinaryMask.h \
    Matcher_TargetMatcherV1.h \
    Matcher_TargetMatcher.h \
    Mat2QImage.h \
    InputChoiceWidget.h \
    ImageDisplayerWidgetImpl3.h \
    ImageDisplayerWidgetImpl2.h \
    ImageDisplayerWidgetImpl.h \
    ImageDisplayerWidget.h \
    GLWidgetRenderingThread.h \
    FrameProcessor.h \
    DiscreteDistribution.h \
    CustomTextInput.h \
    CustomComboBox.h \
    CustomButton.h \
    Clock.h \
    Algorithm_Watershed.h \
    Algorithm_Tagging.h \
    Algorithm_TagAnTrack.h \
    Algorithm_Sub_Tagging.h \
    Algorithm_Sub_BinaryMask.h \
    Algorithm_OomsChallenge.h \
    Algorithm_OomsAlgorithmTest.h \
    Algorithm_NoProcessing.h \
    Algorithm_MeanShift.h \
    Algorithm_FeaturesTracker.h \
    Algorithm_FeatureProjection.h \
    Algorithm_FaceDetector_Surf.h \
    Algorithm_FaceDetector.h \
    Algorithm_EyeFaceDetector.h \
    Algorithm_CustomCondensationTemplateV1.h \
    Algorithm_CustomCondensationV1.h \
    Algorithm_CustomCondensation.h \
    Algorithm_Condensation.h \
    Algorithm_ColorDetection.h \
    Algorithm_BinaryMaskWithOriginalFrame.h \
    Algorithm_CustomCondensationTemplateV1.h \
    Algorithme_CondensationB.h \
    Algorithm_TaggingB.h \
    Algorithm_Sub_TaggingB.h \
    Algorithm_CustomCondensationTemplateV2.h \
    Matcher_GreyLevelDistanceMatcher.h \
    Target_CondensationTargetV2.h \
    Algorithm_AccumulateBackground.h \
    BasicWindow.h \
    CondensationParameters.h



unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

RESOURCES += \
    img.qrc

OTHER_FILES += \
    data/yellowfish.png \
    data/greyfish.png \
    data/bigfish.png \
    img/remove_button.png \
    img/play_icon.png \
    img/pause_icon.png \
    img/fullscreen_icon.png \
    img/add_button.png \
    qss/timeControlSlider.qss \
    qss/QComboBoxDropDown.qss \
    qss/QComboBox.qss \
    utils/sample.py \
    xml/lpbcascade_frontalface.xml \
    xml/haarcascade_frontalface_default.xml \
    xml/haarcascade_frontalface_alt.xml \
    xml/haarcascade_eye_tree_eyeglasses.xml \
    xml/facedata.xml \
    README.md
