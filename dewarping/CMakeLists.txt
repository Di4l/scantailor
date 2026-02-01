#===============================================================================
# Dewarping Library
#===============================================================================
# This library provides image dewarping functionality for correcting
# perspective and cylindrical distortion in scanned documents.

PROJECT("Dewarping library")

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    DewarpingPointMapper.cpp
    DewarpingPointMapper.h
    DistortionModel.cpp
    DistortionModel.h
)

SET(GUI_SOURCES
    gui/Curve.cpp
    gui/Curve.h
    gui/DistortionModelBuilder.cpp
    gui/DistortionModelBuilder.h
    gui/DetectVertContentBounds.cpp
    gui/DetectVertContentBounds.h
    gui/TowardsLineTracer.cpp
    gui/TowardsLineTracer.h
    gui/TextLineTracer.cpp
    gui/TextLineTracer.h
    gui/TextLineRefiner.cpp
    gui/TextLineRefiner.h
    gui/TopBottomEdgeTracer.cpp
    gui/TopBottomEdgeTracer.h
    gui/CylindricalSurfaceDewarper.cpp
    gui/CylindricalSurfaceDewarper.h
    gui/RasterDewarper.cpp
    gui/RasterDewarper.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})
SOURCE_GROUP("GUI Sources" FILES ${GUI_SOURCES})

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(dewarping STATIC ${SOURCES} ${GUI_SOURCES})
