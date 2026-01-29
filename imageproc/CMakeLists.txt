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

# Binary image operations
BinaryImage.cpp BinaryImage.h
BinaryThreshold.cpp BinaryThreshold.h

# Histogram and connectivity
SlicedHistogram.cpp SlicedHistogram.h
ConnComp.h Connectivity.h
ConnCompEraser.cpp ConnCompEraser.h
ConnCompEraserExt.cpp ConnCompEraserExt.h
ConnectivityMap.cpp ConnectivityMap.h

# Seed filling and region operations
SeedFill.cpp SeedFill.h
SeedFillGeneric.cpp SeedFillGeneric.h
InfluenceMap.cpp InfluenceMap.h

# Grayscale operations
GrayImage.cpp GrayImage.h
Grayscale.cpp Grayscale.h
RasterOp.h GrayRasterOp.h RasterOpGeneric.h

# Geometric transformations
UpscaleIntegerTimes.cpp UpscaleIntegerTimes.h
ReduceThreshold.cpp ReduceThreshold.h
Shear.cpp Shear.h
OrthogonalRotation.cpp OrthogonalRotation.h
Scale.cpp Scale.h
Transform.cpp Transform.h

# Feature detection
SkewFinder.cpp SkewFinder.h
HoughLineDetector.cpp HoughLineDetector.h
DentFinder.cpp DentFinder.h
LeastSquaresFit.cpp LeastSquaresFit.h

# Morphological operations
Morphology.cpp Morphology.h
MorphGradientDetect.cpp MorphGradientDetect.h

# Filtering operations
GaussBlur.cpp GaussBlur.h
Sobel.h
SavGolKernel.cpp SavGolKernel.h
SavGolFilter.cpp SavGolFilter.h

# Polynomial fitting
PolynomialLine.cpp PolynomialLine.h
PolynomialSurface.cpp PolynomialSurface.h

# Polygon operations
PolygonUtils.cpp PolygonUtils.h
PolygonRasterizer.cpp PolygonRasterizer.h

# Brightness and color operations
DrawOver.cpp DrawOver.h
AdjustBrightness.cpp AdjustBrightness.h
ColorInterpolation.cpp ColorInterpolation.h

# Advanced image analysis
IntegralImage.h
Binarize.cpp Binarize.h
SEDM.cpp SEDM.h
MaxWhitespaceFinder.cpp MaxWhitespaceFinder.h
LocalMinMaxGeneric.h
FindPeaksGeneric.h
)

SOURCE_GROUP("Sources" FILES ${sources})

#===============================================================================
# BUILD TARGET
#===============================================================================

ADD_LIBRARY(imageproc STATIC ${sources})

#===============================================================================
# SUBDIRECTORIES
#===============================================================================

ADD_SUBDIRECTORY(tests)
