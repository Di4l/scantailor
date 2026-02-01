#===============================================================================
# Foundation Library - Core Utilities and Data Structures
#===============================================================================

PROJECT("Foundation library")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# SOURCE FILES
#===============================================================================

SET(
sources
# Smart pointer and memory management
NonCopyable.h IntrusivePtr.h RefCountable.h SafeDeletingQObjectPtr.h
# Utility containers and helpers
AlignedArray.h
FastQueue.h
ScopedIncDec.h ScopedDecInc.h
Span.h VirtualFunction.h FlagOps.h
# File and property management
Property.h
GridLineTraverser.cpp GridLineTraverser.h
AutoRemovingFile.cpp AutoRemovingFile.h
# Qt-related utilities (non-GUI)
PerformanceTimer.cpp PerformanceTimer.h
PropertyFactory.cpp PropertyFactory.h
PropertySet.cpp PropertySet.h
QtSignalForwarder.cpp QtSignalForwarder.h
Proximity.cpp Proximity.h
# Data structures
StaticPool.h
DynamicPool.h
NumericTraits.h
VecNT.h VecT.h
MatMNT.h MatT.h
PriorityQueue.h
Grid.h
ValueConv.h
MultiIndexContainer.h
)

SOURCE_GROUP("Sources" FILES ${sources})

#===============================================================================
# Qt5 AUTOMATIC MOC
#===============================================================================
SET(CMAKE_AUTOMOC ON)

#===============================================================================
# BUILD TARGET
#===============================================================================

ADD_LIBRARY(foundation STATIC ${sources})
