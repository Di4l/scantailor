#===============================================================================
# Image Processing Library - Core Image Manipulation Algorithms
#===============================================================================

PROJECT(imageproc)

#===============================================================================
# SOURCE FILES - Image Processing Algorithms
#===============================================================================

SET(
sources
# Constants and basic utilities
Constants.h Constants.cpp
ByteOrder.h BWColor.h ColorMixer.h ColorForId.h
BitOps.cpp BitOps.h

# Histogram and connectivity (CORE)
SlicedHistogram.cpp SlicedHistogram.h
ConnComp.h Connectivity.h
ConnCompEraser.cpp ConnCompEraser.h
ConnCompEraserExt.cpp ConnCompEraserExt.h

# Seed filling and region operations (CORE)
SeedFillGeneric.cpp SeedFillGeneric.h

# Raster operations (CORE)
RasterOp.h GrayRasterOp.h RasterOpGeneric.h

# Geometric transformations (CORE only)
UpscaleIntegerTimes.cpp UpscaleIntegerTimes.h
ReduceThreshold.cpp ReduceThreshold.h
Shear.cpp Shear.h
OrthogonalRotation.cpp OrthogonalRotation.h

# Feature detection (CORE only)
SkewFinder.cpp SkewFinder.h
DentFinder.cpp DentFinder.h
LeastSquaresFit.cpp LeastSquaresFit.h

# Morphological operations (CORE)
Morphology.cpp Morphology.h

# Filtering operations (CORE)
GaussBlur.cpp GaussBlur.h
Sobel.h
SavGolKernel.cpp SavGolKernel.h

# Polynomial fitting
PolynomialLine.cpp PolynomialLine.h
PolynomialSurface.cpp PolynomialSurface.h

# Advanced image analysis (CORE)
IntegralImage.h
SEDM.cpp SEDM.h
MaxWhitespaceFinder.cpp MaxWhitespaceFinder.h
LocalMinMaxGeneric.h
FindPeaksGeneric.h

# Brightness and color operations (CORE)
ColorInterpolation.cpp ColorInterpolation.h
)

SET(
gui_sources
# Binary image operations
gui/BinaryImage.cpp gui/BinaryImage.h
gui/BinaryThreshold.cpp gui/BinaryThreshold.h

# Seed filling and region operations
gui/SeedFill.cpp gui/SeedFill.h
gui/InfluenceMap.cpp gui/InfluenceMap.h

# Connectivity operations
gui/ConnectivityMap.cpp gui/ConnectivityMap.h

# Grayscale operations
gui/GrayImage.cpp gui/GrayImage.h
gui/Grayscale.cpp gui/Grayscale.h

# Geometric transformations (GUI)
gui/Scale.cpp gui/Scale.h
gui/Transform.cpp gui/Transform.h

# Feature detection (GUI)
gui/HoughLineDetector.cpp gui/HoughLineDetector.h

# Morphological operations (GUI)
gui/MorphGradientDetect.cpp gui/MorphGradientDetect.h

# Filtering operations (GUI)
gui/SavGolFilter.cpp gui/SavGolFilter.h

# Polygon operations
gui/PolygonUtils.cpp gui/PolygonUtils.h
gui/PolygonRasterizer.cpp gui/PolygonRasterizer.h

# Brightness and color operations
gui/DrawOver.cpp gui/DrawOver.h
gui/AdjustBrightness.cpp gui/AdjustBrightness.h

# Advanced image analysis (GUI)
gui/Binarize.cpp gui/Binarize.h
)

SOURCE_GROUP("Sources" FILES ${sources})
SOURCE_GROUP("GUI Sources" FILES ${gui_sources})

#===============================================================================
# BUILD TARGET
#===============================================================================

ADD_LIBRARY(imageproc STATIC ${sources} ${gui_sources})

#===============================================================================
# SUBDIRECTORIES
#===============================================================================

ADD_SUBDIRECTORY(tests)
