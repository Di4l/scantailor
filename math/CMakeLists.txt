#===============================================================================
# Math Library - Mathematical Algorithms and Transformations
#===============================================================================

PROJECT("Math library")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# SOURCE FILES - Core Math Functions
#===============================================================================

SET(
GENERIC_SOURCES
# Linear algebra and solvers
LinearSolver.cpp LinearSolver.h
MatrixCalc.h
HomographicTransform.h

# Geometric calculations
SidesOfLine.cpp SidesOfLine.h
ToLineProjector.cpp ToLineProjector.h
ArcLengthMapper.cpp ArcLengthMapper.h
LineIntersectionScalar.cpp LineIntersectionScalar.h
LineBoundedByRect.cpp LineBoundedByRect.h
PolylineIntersector.cpp PolylineIntersector.h

# Function representations
LinearFunction.cpp LinearFunction.h
QuadraticFunction.cpp QuadraticFunction.h
XSpline.cpp XSpline.h
)

SOURCE_GROUP("Sources" FILES ${GENERIC_SOURCES})

#===============================================================================
# SOURCE FILES - Spline Fitting Framework
#===============================================================================

SET(
SPFIT_SOURCES
spfit/references.txt
spfit/FittableSpline.h
spfit/FrenetFrame.cpp spfit/FrenetFrame.h
spfit/ConstraintSet.cpp spfit/ConstraintSet.h
spfit/SqDistApproximant.cpp spfit/SqDistApproximant.h
spfit/ModelShape.h
spfit/PolylineModelShape.cpp spfit/PolylineModelShape.h
spfit/LinearForceBalancer.cpp spfit/LinearForceBalancer.h
spfit/OptimizationResult.cpp spfit/OptimizationResult.h
spfit/Optimizer.cpp spfit/Optimizer.h
spfit/SplineFitter.cpp spfit/SplineFitter.h
)

SOURCE_GROUP("Sources\\Spline Fitting Framework" FILES ${SPFIT_SOURCES})

#===============================================================================
# SOURCE FILES - Automatic Differentiation Framework
#===============================================================================

SET(
ADIFF_SOURCES
adiff/references.txt
adiff/SparseMap.cpp adiff/SparseMap.h
adiff/Function.cpp adiff/Function.h
)

SOURCE_GROUP("Sources\\Differentiation Framework" FILES ${ADIFF_SOURCES})

#===============================================================================
# BUILD TARGET
#===============================================================================

ADD_LIBRARY(math STATIC ${GENERIC_SOURCES} ${SPFIT_SOURCES} ${ADIFF_SOURCES})

#===============================================================================
# SUBDIRECTORIES - TESTS
#===============================================================================

ADD_SUBDIRECTORY(spfit/tests)
ADD_SUBDIRECTORY(adiff/tests)
