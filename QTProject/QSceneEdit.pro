#-------------------------------------------------
#
# Project created by QtCreator 2012-10-13T21:34:29
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QSceneEdit
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    trimesh/TriMesh_tstrips.cc \
    trimesh/TriMesh_stats.cc \
    trimesh/TriMesh_pointareas.cc \
    trimesh/TriMesh_normals.cc \
    trimesh/TriMesh_io.cc \
    trimesh/TriMesh_grid.cc \
    trimesh/TriMesh_curvature.cc \
    trimesh/TriMesh_connectivity.cc \
    trimesh/TriMesh_bounding.cc \
    trimesh/subdiv.cc \
    trimesh/shared.cc \
    trimesh/reorder_verts.cc \
    trimesh/remove.cc \
    trimesh/overlap.cc \
    trimesh/lmsmooth.cc \
    trimesh/KDtree.cc \
    trimesh/ICP.cc \
    trimesh/GLCamera.cc \
    trimesh/filter.cc \
    trimesh/faceflip.cc \
    trimesh/edgeflip.cc \
    trimesh/diffuse.cc \
    trimesh/conn_comps.cc \
    3DFeatureExtract/Bitmap.cpp \
    3DFeatureExtract/Circularity.cpp \
    3DFeatureExtract/ColorDescriptor.cpp \
    3DFeatureExtract/CompactColor.c \
    3DFeatureExtract/Convert.cpp \
    3DFeatureExtract/Eccentricity.cpp \
    3DFeatureExtract/Edge.cpp \
    3DFeatureExtract/FeatureExtract.cpp \
    3DFeatureExtract/FourierDescriptor.cpp \
    3DFeatureExtract/MORPHOLOGY.cpp \
    3DFeatureExtract/RecovAffine.cpp \
    3DFeatureExtract/RegionShape.cpp \
    3DFeatureExtract/Rotate.cpp \
    3DFeatureExtract/RWObj.cpp \
    3DFeatureExtract/thin.cpp \
    3DFeatureExtract/TraceContour.cpp \
    3DFeatureExtract/TranslateScale.cpp \
    3DFeatureExtract/fftw/config.cpp \
    3DFeatureExtract/fftw/fhf_2.cpp \
    3DFeatureExtract/fftw/fcr_7.cpp \
    3DFeatureExtract/fftw/fcr_8.cpp \
    3DFeatureExtract/fftw/fcr_9.cpp \
    3DFeatureExtract/fftw/fcr_11.cpp \
    3DFeatureExtract/fftw/fcr_12.cpp \
    3DFeatureExtract/fftw/fcr_13.cpp \
    3DFeatureExtract/fftw/fcr_64.cpp \
    3DFeatureExtract/fftw/fcr_128.cpp \
    3DFeatureExtract/fftw/fftwnd.cpp \
    3DFeatureExtract/fftw/fhb_2.cpp \
    3DFeatureExtract/fftw/fhb_3.cpp \
    3DFeatureExtract/fftw/fhb_4.cpp \
    3DFeatureExtract/fftw/fhb_6.cpp \
    3DFeatureExtract/fftw/fhb_7.cpp \
    3DFeatureExtract/fftw/fhb_8.cpp \
    3DFeatureExtract/fftw/fhb_9.cpp \
    3DFeatureExtract/fftw/fhb_10.cpp \
    3DFeatureExtract/fftw/fhb_16.cpp \
    3DFeatureExtract/fftw/fhb_32.cpp \
    3DFeatureExtract/fftw/fhf_3.cpp \
    3DFeatureExtract/fftw/fhf_10.cpp \
    3DFeatureExtract/fftw/fni_15.cpp \
    3DFeatureExtract/fftw/executor.cpp \
    3DFeatureExtract/fftw/fcr_3.cpp \
    3DFeatureExtract/fftw/fcr_4.cpp \
    3DFeatureExtract/fftw/fcr_5.cpp \
    3DFeatureExtract/fftw/fcr_14.cpp \
    3DFeatureExtract/fftw/fcr_16.cpp \
    3DFeatureExtract/fftw/fcr_32.cpp \
    3DFeatureExtract/fftw/fni_2.cpp \
    3DFeatureExtract/fftw/fni_3.cpp \
    3DFeatureExtract/fftw/fni_4.cpp \
    3DFeatureExtract/fftw/fni_16.cpp \
    3DFeatureExtract/fftw/fni_32.cpp \
    3DFeatureExtract/fftw/fcr_1.cpp \
    3DFeatureExtract/fftw/fcr_6.cpp \
    3DFeatureExtract/fftw/fcr_10.cpp \
    3DFeatureExtract/fftw/fni_5.cpp \
    3DFeatureExtract/fftw/frc_2.cpp \
    3DFeatureExtract/fftw/frc_6.cpp \
    3DFeatureExtract/fftw/frc_7.cpp \
    3DFeatureExtract/fftw/ftw_4.cpp \
    3DFeatureExtract/fftw/ftw_5.cpp \
    3DFeatureExtract/fftw/ftw_8.cpp \
    3DFeatureExtract/fftw/fn_1.cpp \
    3DFeatureExtract/fftw/fn_3.cpp \
    3DFeatureExtract/fftw/fn_4.cpp \
    3DFeatureExtract/fftw/fn_5.cpp \
    3DFeatureExtract/fftw/fn_6.cpp \
    3DFeatureExtract/fftw/fn_10.cpp \
    3DFeatureExtract/fftw/fn_11.cpp \
    3DFeatureExtract/fftw/fn_12.cpp \
    3DFeatureExtract/fftw/fn_14.cpp \
    3DFeatureExtract/fftw/fn_32.cpp \
    3DFeatureExtract/fftw/fn_64.cpp \
    3DFeatureExtract/fftw/fni_6.cpp \
    3DFeatureExtract/fftw/fni_7.cpp \
    3DFeatureExtract/fftw/fni_8.cpp \
    3DFeatureExtract/fftw/fni_9.cpp \
    3DFeatureExtract/fftw/fni_64.cpp \
    3DFeatureExtract/fftw/frc_8.cpp \
    3DFeatureExtract/fftw/frc_9.cpp \
    3DFeatureExtract/fftw/frc_10.cpp \
    3DFeatureExtract/fftw/frc_11.cpp \
    3DFeatureExtract/fftw/frc_12.cpp \
    3DFeatureExtract/fftw/frc_13.cpp \
    3DFeatureExtract/fftw/frc_14.cpp \
    3DFeatureExtract/fftw/frc_15.cpp \
    3DFeatureExtract/fftw/frc_16.cpp \
    3DFeatureExtract/fftw/frc_128.cpp \
    3DFeatureExtract/fftw/ftwi_2.cpp \
    3DFeatureExtract/fftw/ftwi_7.cpp \
    3DFeatureExtract/fftw/ftwi_10.cpp \
    3DFeatureExtract/fftw/ftw_2.cpp \
    3DFeatureExtract/fftw/ftw_3.cpp \
    3DFeatureExtract/fftw/ftw_6.cpp \
    3DFeatureExtract/fftw/ftw_7.cpp \
    3DFeatureExtract/fftw/ftw_10.cpp \
    3DFeatureExtract/fftw/ftw_16.cpp \
    3DFeatureExtract/fftw/ftw_32.cpp \
    3DFeatureExtract/fftw/ftw_64.cpp \
    3DFeatureExtract/fftw/ftwi_9.cpp \
    3DFeatureExtract/fftw/timer.cpp \
    3DFeatureExtract/fftw/twiddle.cpp \
    3DFeatureExtract/fftw/wisdom.cpp \
    3DFeatureExtract/fftw/wisdomio.cpp \
    3DFeatureExtract/fftw/fhf_5.cpp \
    3DFeatureExtract/fftw/fhf_6.cpp \
    3DFeatureExtract/fftw/fni_10.cpp \
    3DFeatureExtract/fftw/fni_11.cpp \
    3DFeatureExtract/fftw/fni_12.cpp \
    3DFeatureExtract/fftw/fni_13.cpp \
    3DFeatureExtract/fftw/fni_14.cpp \
    3DFeatureExtract/fftw/frc_3.cpp \
    3DFeatureExtract/fftw/frc_4.cpp \
    3DFeatureExtract/fftw/frc_5.cpp \
    3DFeatureExtract/fftw/frc_32.cpp \
    3DFeatureExtract/fftw/frc_64.cpp \
    3DFeatureExtract/fftw/ftw_9.cpp \
    3DFeatureExtract/fftw/ftwi_3.cpp \
    3DFeatureExtract/fftw/ftwi_4.cpp \
    3DFeatureExtract/fftw/ftwi_16.cpp \
    3DFeatureExtract/fftw/ftwi_32.cpp \
    3DFeatureExtract/fftw/generic.cpp \
    3DFeatureExtract/fftw/malloc.cpp \
    3DFeatureExtract/fftw/rfftwnd.cpp \
    3DFeatureExtract/fftw/rgeneric.cpp \
    3DFeatureExtract/fftw/rplanner.cpp \
    3DFeatureExtract/fftw/fcr_2.cpp \
    3DFeatureExtract/fftw/fcr_15.cpp \
    3DFeatureExtract/fftw/fftwf77.cpp \
    3DFeatureExtract/fftw/fhb_5.cpp \
    3DFeatureExtract/fftw/fhf_4.cpp \
    3DFeatureExtract/fftw/fhf_7.cpp \
    3DFeatureExtract/fftw/fhf_8.cpp \
    3DFeatureExtract/fftw/fhf_9.cpp \
    3DFeatureExtract/fftw/fhf_16.cpp \
    3DFeatureExtract/fftw/fhf_32.cpp \
    3DFeatureExtract/fftw/fn_2.cpp \
    3DFeatureExtract/fftw/fn_7.cpp \
    3DFeatureExtract/fftw/fn_8.cpp \
    3DFeatureExtract/fftw/fn_9.cpp \
    3DFeatureExtract/fftw/fn_13.cpp \
    3DFeatureExtract/fftw/fn_15.cpp \
    3DFeatureExtract/fftw/fn_16.cpp \
    3DFeatureExtract/fftw/fni_1.cpp \
    3DFeatureExtract/fftw/frc_1.cpp \
    3DFeatureExtract/fftw/ftwi_5.cpp \
    3DFeatureExtract/fftw/ftwi_6.cpp \
    3DFeatureExtract/fftw/ftwi_8.cpp \
    3DFeatureExtract/fftw/ftwi_64.cpp \
    3DFeatureExtract/fftw/planner.cpp \
    3DFeatureExtract/fftw/putils.cpp \
    3DFeatureExtract/fftw/rader.cpp \
    3DFeatureExtract/fftw/rconfig.cpp \
    3DFeatureExtract/fftw/rexec.cpp \
    3DFeatureExtract/fftw/rexec2.cpp \
    3DFeatureExtract/fftw/rfftwf77.cpp \
    3DSearch/search.cpp \
    Global.cpp \
    QSceneDisplay.cpp \
    mersenne-twister.cpp \
    MathLib.cpp \
    Model.cpp \
    Material.cpp \
    Scene.cpp \
    QPictureDisplay.cpp \
    QSameModelListDialog.cpp \
    QSegPictureDisplay.cpp \
    QSetRelationDialog.cpp \
    QSearchListDisplay.cpp \
    QTagDisplay.cpp \
    QModelListDialog.cpp \
    ImageSeg/SegObject.cpp \
    ImageSeg/MeanShift.cpp \
    ImageSeg/CvvImage.cpp \
    NaiveBayesClassify/NB.cpp

HEADERS  += mainwindow.h \
    trimesh/XForm.h \
    trimesh/Vec.h \
    trimesh/TriMesh_algo.h \
    trimesh/TriMesh.h \
    trimesh/timestamp.h \
    trimesh/strutil.h \
    trimesh/noise3d.h \
    trimesh/mempool.h \
    trimesh/lineqn.h \
    trimesh/KDtree.h \
    trimesh/ICP.h \
    trimesh/GLCamera.h \
    trimesh/Color.h \
    trimesh/bsphere.h \
    trimesh/Box.h \
    3DFeatureExtract/Bitmap.h \
    3DFeatureExtract/Circularity.h \
    3DFeatureExtract/ColorDescriptor.h \
    3DFeatureExtract/convert.h \
    3DFeatureExtract/Ds.h \
    3DFeatureExtract/Eccentricity.h \
    3DFeatureExtract/Edge.h \
    3DFeatureExtract/FeatureExtract.h \
    3DFeatureExtract/FourierDescriptor.h \
    3DFeatureExtract/MORPHOLOGY.h \
    3DFeatureExtract/RecovAffine.h \
    3DFeatureExtract/RegionShape.h \
    3DFeatureExtract/Rotate.h \
    3DFeatureExtract/RWObj.h \
    3DFeatureExtract/thin.h \
    3DFeatureExtract/TraceContour.h \
    3DFeatureExtract/TranslateScale.h \
    3DFeatureExtract/fftw/config.h \
    3DFeatureExtract/fftw/f77_func.h \
    3DFeatureExtract/fftw/fftw.h \
    3DFeatureExtract/fftw/fftw-int.h \
    3DFeatureExtract/fftw/rfftw.h \
    3DSearch/ds.h \
    3DSearch/search.h \
    Global.h \
    mersenne-twister.h \
    MathLib.h \
    Model.h \
    Material.h \
    BaseStruct.h \
    BaseFunction.h \
    Scene.h \
    QSceneDisplay.h \
    QPictureDisplay.h \
    QSameModelListDialog.h \
    QSegPictureDisplay.h \
    QSetRelationDialog.h \
    QSearchListDisplay.h \
    QModelListDialog.h \
    QTagDisplay.h \
    ImageSeg/SegObject.h \
    ImageSeg/MeanShift.h \
    ImageSeg/CvvImage.h \
    NaiveBayesClassify/NB.h

FORMS    += mainwindow.ui

RESOURCES += \
    QSceneEdit.qrc


INCLUDEPATH +=  C:/OpenCV2.2/include/opencv

INCLUDEPATH +=  C:/OpenCV2.2/include

LIBS +=  C:/OpenCV2.2/lib/*
