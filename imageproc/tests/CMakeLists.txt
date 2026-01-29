#===============================================================================
# Image Processing Tests
#===============================================================================
# Unit tests for image processing algorithms and transformations.
# These tests validate the correctness of various imageproc library functions.

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES(BEFORE ..)

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    main.cpp
    TestBinaryImage.cpp
    TestReduceThreshold.cpp
    TestSlicedHistogram.cpp
    TestConnCompEraser.cpp
    TestConnCompEraserExt.cpp
    TestGrayscale.cpp
    TestRasterOp.cpp
    TestShear.cpp
    TestOrthogonalRotation.cpp
    TestSkewFinder.cpp
    TestScale.cpp
    TestTransform.cpp
    TestMorphology.cpp
    TestDentFinder.cpp
    TestBinarize.cpp
    TestPolygonRasterizer.cpp
    TestSeedFill.cpp
    TestSEDM.cpp
    Utils.cpp
    Utils.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Link Libraries
#===============================================================================

SET(
    LIBS
    imageproc
    math
    foundation
    Qt5::Gui
    Qt5::Core
    ${EXTRA_LIBS}
)

#===============================================================================
# Build Target
#===============================================================================

ADD_EXECUTABLE(imageproc_tests ${SOURCES})
TARGET_LINK_LIBRARIES(imageproc_tests ${LIBS})
