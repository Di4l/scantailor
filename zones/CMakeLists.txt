#===============================================================================
# Zones Library
#===============================================================================
# This library provides spline-based zone management for document editing,
# including editable zones with serialization and user interactions.

PROJECT(zones)

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    SplineVertex.cpp
    SplineVertex.h
    SplineSegment.cpp
    SplineSegment.h
    Zone.cpp
    Zone.h
    ZoneSet.cpp
    ZoneSet.h
    ZoneInteractionContext.cpp
    ZoneInteractionContext.h
)

SET(GUI_SOURCES
    EditableSpline.cpp
    EditableSpline.h
    SerializableSpline.cpp
    SerializableSpline.h
    EditableZoneSet.cpp
    EditableZoneSet.h
    BasicSplineVisualizer.cpp
    BasicSplineVisualizer.h
    ZoneDefaultInteraction.cpp
    ZoneDefaultInteraction.h
    ZoneCreationInteraction.cpp
    ZoneCreationInteraction.h
    ZoneVertexDragInteraction.cpp
    ZoneVertexDragInteraction.h
    ZoneContextMenuInteraction.cpp
    ZoneContextMenuInteraction.h
    ZoneContextMenuItem.h
)

SET(SOURCES ${SOURCES} gui/${GUI_SOURCES})

SOURCE_GROUP(Sources FILES ${SOURCES})

#===============================================================================
# Qt5 Meta-Object Compilation
#===============================================================================
# Qt5 uses automatic moc, no need for explicit QT5_WRAP_CPP

SET(CMAKE_AUTOMOC ON)

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(zones STATIC ${SOURCES})

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES})