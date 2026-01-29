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
    Curve.cpp
    Curve.h
    DistortionModel.cpp
    DistortionModel.h
    DistortionModelBuilder.cpp
    DistortionModelBuilder.h
    DetectVertContentBounds.cpp
    DetectVertContentBounds.h
    TowardsLineTracer.cpp
    TowardsLineTracer.h
    TextLineTracer.cpp
    TextLineTracer.h
    TextLineRefiner.cpp
    TextLineRefiner.h
    TopBottomEdgeTracer.cpp
    TopBottomEdgeTracer.h
    CylindricalSurfaceDewarper.cpp
    CylindricalSurfaceDewarper.h
    DewarpingPointMapper.cpp
    DewarpingPointMapper.h
    RasterDewarper.cpp
    RasterDewarper.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(dewarping STATIC ${SOURCES})
