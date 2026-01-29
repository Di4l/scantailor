#===============================================================================
# Page Split Filter
#===============================================================================
# This filter handles automatic page splitting for double-page scans.
# It detects split lines and provides layout estimation capabilities.

PROJECT("Page Split Filter")

#===============================================================================
# Configure Directories
#===============================================================================

INCLUDE_DIRECTORIES(BEFORE "${CMAKE_CURRENT_BINARY_DIR}")

#===============================================================================
# UI Files (Qt Designer)
#===============================================================================

FILE(GLOB UI_FILES "ui/*.ui")
QT5_WRAP_UI(UI_SOURCES ${UI_FILES})
SET_SOURCE_FILES_PROPERTIES(${UI_SOURCES} PROPERTIES GENERATED TRUE)

SOURCE_GROUP("UI Files" FILES ${UI_FILES})
SOURCE_GROUP("Generated" FILES ${UI_SOURCES})

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    SplitLineObject.h
    ImageView.cpp
    ImageView.h
    Thumbnail.cpp
    Thumbnail.h
    Params.cpp
    Params.h
    Dependencies.cpp
    Dependencies.h
    PageLayout.cpp
    PageLayout.h
    PageLayoutEstimator.cpp
    PageLayoutEstimator.h
    VertLineFinder.cpp
    VertLineFinder.h
    Filter.cpp
    Filter.h
    OptionsWidget.cpp
    OptionsWidget.h
    SplitModeDialog.cpp
    SplitModeDialog.h
    Settings.cpp
    Settings.h
    Task.cpp
    Task.h
    CacheDrivenTask.cpp
    CacheDrivenTask.h
    LayoutType.cpp
    LayoutType.h
    UnremoveButton.cpp
    UnremoveButton.h
    OrderBySplitTypeProvider.cpp
    OrderBySplitTypeProvider.h
)

SOURCE_GROUP("Sources" FILES ${SOURCES})

#===============================================================================
# Qt5 Meta-Object Compilation
#===============================================================================
# Qt5 uses automatic moc, no need for explicit QT5_WRAP_CPP

SET(CMAKE_AUTOMOC ON)

#===============================================================================
# Build Target
#===============================================================================

ADD_LIBRARY(page_split STATIC ${SOURCES} ${UI_SOURCES})

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})