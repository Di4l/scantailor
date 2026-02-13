#===============================================================================
# Foundation Library - Core Utilities and Data Structures
#===============================================================================

PROJECT("Foundation library")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# SOURCE FILES
#===============================================================================

SET(SOURCES
# Smart pointer and memory management
NonCopyable.h IntrusivePtr.h RefCountable.h SafeDeletingQObjectPtr.h
# Utility containers and helpers
AlignedArray.h
FastQueue.h
ScopedIncDec.h ScopedDecInc.h
Span.h VirtualFunction.h FlagOps.h
# File and property management
qt/Property.h
qt/GridLineTraverser.cpp qt/GridLineTraverser.h
qt/AutoRemovingFile.cpp qt/AutoRemovingFile.h
# Qt-related utilities (non-GUI)
qt/PerformanceTimer.cpp qt/PerformanceTimer.h
qt/PropertyFactory.cpp qt/PropertyFactory.h
qt/PropertySet.cpp qt/PropertySet.h
qt/QtSignalForwarder.cpp qt/QtSignalForwarder.h
qt/Proximity.cpp qt/Proximity.h
# Data structures
StaticPool.h
DynamicPool.h
NumericTraits.h
qt/VecNT.h VecT.h
MatMNT.h MatT.h
PriorityQueue.h
Grid.h
ValueConv.h
MultiIndexContainer.h
qt/VecNT.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Qt5 AUTOMATIC MOC
#===============================================================================
SET(CMAKE_AUTOMOC ON)

#===============================================================================
# BUILD TARGET
#===============================================================================

ADD_LIBRARY(foundation STATIC ${SOURCES})
