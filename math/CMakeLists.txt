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
# Linear algebra and solvers (CORE)
LinearSolver.cpp LinearSolver.h
MatrixCalc.h
HomographicTransform.h

# Function representations (CORE)
LinearFunction.cpp LinearFunction.h
QuadraticFunction.cpp QuadraticFunction.h
ArcLengthMapper.cpp ArcLengthMapper.h
)

SET(
GENERIC_GUI_SOURCES
# Geometric calculations (GUI)
gui/SidesOfLine.cpp gui/SidesOfLine.h
gui/ToLineProjector.cpp gui/ToLineProjector.h
gui/LineIntersectionScalar.cpp gui/LineIntersectionScalar.h
gui/LineBoundedByRect.cpp gui/LineBoundedByRect.h
gui/PolylineIntersector.cpp gui/PolylineIntersector.h

# Function representations (GUI)
gui/XSpline.cpp gui/XSpline.h
)

SOURCE_GROUP("Sources" FILES ${GENERIC_SOURCES})
SOURCE_GROUP("GUI Sources" FILES ${GENERIC_GUI_SOURCES})

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

ADD_LIBRARY(math STATIC ${GENERIC_SOURCES} ${GENERIC_GUI_SOURCES} ${SPFIT_SOURCES} ${ADIFF_SOURCES})

#===============================================================================
# SUBDIRECTORIES - TESTS
#===============================================================================
