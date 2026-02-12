#===============================================================================
# Output Filter
#===============================================================================
# This filter handles final output generation, color management, despeckling,
# dewarping, and various image processing options for document output.

PROJECT("Output Filter")

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
    CacheDrivenTask.h
    Filter.h
    qt/CacheDrivenTask.cpp
    qt/Task.h
    qt/ApplyColorsDialog.cpp
    qt/ApplyColorsDialog.h
    qt/ChangeDpiDialog.cpp
    qt/ChangeDpiDialog.h
    qt/ImageView.cpp
    qt/ImageView.h
    qt/ImageViewTab.h
    qt/TabbedImageView.cpp
    qt/TabbedImageView.h
    qt/Filter.cpp
    qt/OptionsWidget.cpp
    qt/OptionsWidget.h
    qt/Task.cpp
    qt/OutputGenerator.cpp
    qt/OutputGenerator.h
    qt/OutputMargins.h
    qt/Settings.cpp
    qt/Settings.h
    qt/Thumbnail.cpp
    qt/Thumbnail.h
    qt/Utils.cpp
    qt/Utils.h
    qt/Params.cpp
    qt/Params.h
    qt/BlackWhiteOptions.cpp
    qt/BlackWhiteOptions.h
    qt/ColorGrayscaleOptions.cpp
    qt/ColorGrayscaleOptions.h
    qt/RenderParams.cpp
    qt/RenderParams.h
    qt/ColorParams.cpp
    qt/ColorParams.h
    qt/OutputImageParams.cpp
    qt/OutputImageParams.h
    qt/OutputFileParams.cpp
    qt/OutputFileParams.h
    qt/OutputParams.cpp
    qt/OutputParams.h
    qt/PictureLayerProperty.cpp
    qt/PictureLayerProperty.h
    qt/PictureZonePropFactory.cpp
    qt/PictureZonePropFactory.h
    qt/PictureZonePropDialog.cpp
    qt/PictureZonePropDialog.h
    qt/PictureZoneComparator.cpp
    qt/PictureZoneComparator.h
    qt/PictureZoneEditor.cpp
    qt/PictureZoneEditor.h
    qt/FillColorProperty.cpp
    qt/FillColorProperty.h
    qt/FillZonePropFactory.cpp
    qt/FillZonePropFactory.h
    qt/FillZoneComparator.cpp
    qt/FillZoneComparator.h
    qt/FillZoneEditor.cpp
    qt/FillZoneEditor.h
    qt/ColorPickupInteraction.cpp
    qt/ColorPickupInteraction.h
    qt/DespeckleState.cpp
    qt/DespeckleState.h
    qt/DespeckleView.cpp
    qt/DespeckleView.h
    qt/DespeckleVisualization.cpp
    qt/DespeckleVisualization.h
    qt/DespeckleLevel.cpp
    qt/DespeckleLevel.h
    qt/DewarpingView.cpp
    qt/DewarpingView.h
    qt/DewarpingMode.cpp
    qt/DewarpingMode.h
    qt/ChangeDewarpingDialog.cpp
    qt/ChangeDewarpingDialog.h
    qt/DepthPerception.cpp
    qt/DepthPerception.h
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

ADD_LIBRARY(output STATIC ${SOURCES} ${UI_SOURCES})
ADD_DEPENDENCIES(output toplevel_ui_sources)

target_include_directories(output PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})