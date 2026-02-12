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
    qt/DewarpingPointMapper.cpp
    qt/DewarpingPointMapper.h
    qt/DistortionModel.cpp
    qt/DistortionModel.h
)

SET(GUI_SOURCES
    qt/gui/Curve.cpp
    qt/gui/Curve.h
    qt/gui/DistortionModelBuilder.cpp
    qt/gui/DistortionModelBuilder.h
    qt/gui/DetectVertContentBounds.cpp
    qt/gui/DetectVertContentBounds.h
    qt/gui/TowardsLineTracer.cpp
    qt/gui/TowardsLineTracer.h
    qt/gui/TextLineTracer.cpp
    qt/gui/TextLineTracer.h
    qt/gui/TextLineRefiner.cpp
    qt/gui/TextLineRefiner.h
    qt/gui/TopBottomEdgeTracer.cpp
    qt/gui/TopBottomEdgeTracer.h
    qt/gui/CylindricalSurfaceDewarper.cpp
    qt/gui/CylindricalSurfaceDewarper.h
    qt/gui/RasterDewarper.cpp
    qt/gui/RasterDewarper.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})
SOURCE_GROUP("GUI Sources" FILES ${GUI_SOURCES})

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(dewarping STATIC ${SOURCES} ${GUI_SOURCES})
