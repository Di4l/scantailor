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

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/qt/ui
)

#===============================================================================
# UI Files (Qt Designer)
#===============================================================================

FILE(GLOB UI_FILES "qt/ui/*.ui")
QT5_WRAP_UI(UI_SOURCES ${UI_FILES})
SET_SOURCE_FILES_PROPERTIES(${UI_SOURCES} PROPERTIES GENERATED TRUE)

SOURCE_GROUP("UI Files" FILES ${UI_FILES})
SOURCE_GROUP("Generated" FILES ${UI_SOURCES})

#===============================================================================
# Source Files
#===============================================================================

SET(SOURCES
    CacheDrivenTask.cpp
    CacheDrivenTask.h
    qt/Filter.h
    Task.h
    qt/SplitLineObject.h
    qt/ImageView.cpp
    qt/ImageView.h
    qt/Thumbnail.cpp
    qt/Thumbnail.h
    qt/Params.cpp
    qt/Params.h
    qt/Dependencies.cpp
    qt/Dependencies.h
    qt/PageLayout.cpp
    qt/PageLayout.h
    qt/PageLayoutEstimator.cpp
    qt/PageLayoutEstimator.h
    qt/VertLineFinder.cpp
    qt/VertLineFinder.h
    qt/Filter.cpp
    qt/OptionsWidget.cpp
    qt/OptionsWidget.h
    qt/SplitModeDialog.cpp
    qt/SplitModeDialog.h
    qt/Settings.cpp
    qt/Settings.h
    qt/Task.cpp
    qt/LayoutType.cpp
    qt/LayoutType.h
    qt/UnremoveButton.cpp
    qt/UnremoveButton.h
    qt/OrderBySplitTypeProvider.cpp
    qt/OrderBySplitTypeProvider.h
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

target_include_directories(page_split PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})