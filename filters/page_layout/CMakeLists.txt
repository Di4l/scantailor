#===============================================================================
# Page Layout Filter
#===============================================================================
# This filter handles page layout configuration and margin adjustment.
# It provides tools for defining page boundaries and layout parameters.

PROJECT("Page Layout Filter")

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
    Utils.cpp
    Utils.h
    ImageView.cpp
    ImageView.h
    Filter.cpp
    Filter.h
    OptionsWidget.cpp
    OptionsWidget.h
    Task.cpp
    Task.h
    CacheDrivenTask.cpp
    CacheDrivenTask.h
    Params.cpp
    Params.h
    Settings.cpp
    Settings.h
    Thumbnail.cpp
    Thumbnail.h
    ApplyDialog.cpp
    ApplyDialog.h
    Alignment.cpp
    Alignment.h
    OrderByWidthProvider.cpp
    OrderByWidthProvider.h
    OrderByHeightProvider.cpp
    OrderByHeightProvider.h
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

ADD_LIBRARY(page_layout STATIC ${SOURCES} ${UI_SOURCES})

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})