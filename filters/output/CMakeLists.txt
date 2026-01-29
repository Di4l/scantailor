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
    ApplyColorsDialog.cpp
    ApplyColorsDialog.h
    ChangeDpiDialog.cpp
    ChangeDpiDialog.h
    ImageView.cpp
    ImageView.h
    ImageViewTab.h
    TabbedImageView.cpp
    TabbedImageView.h
    Filter.cpp
    Filter.h
    OptionsWidget.cpp
    OptionsWidget.h
    Task.cpp
    Task.h
    CacheDrivenTask.cpp
    CacheDrivenTask.h
    OutputGenerator.cpp
    OutputGenerator.h
    OutputMargins.h
    Settings.cpp
    Settings.h
    Thumbnail.cpp
    Thumbnail.h
    Utils.cpp
    Utils.h
    Params.cpp
    Params.h
    BlackWhiteOptions.cpp
    BlackWhiteOptions.h
    ColorGrayscaleOptions.cpp
    ColorGrayscaleOptions.h
    RenderParams.cpp
    RenderParams.h
    ColorParams.cpp
    ColorParams.h
    OutputImageParams.cpp
    OutputImageParams.h
    OutputFileParams.cpp
    OutputFileParams.h
    OutputParams.cpp
    OutputParams.h
    PictureLayerProperty.cpp
    PictureLayerProperty.h
    PictureZonePropFactory.cpp
    PictureZonePropFactory.h
    PictureZonePropDialog.cpp
    PictureZonePropDialog.h
    PictureZoneComparator.cpp
    PictureZoneComparator.h
    PictureZoneEditor.cpp
    PictureZoneEditor.h
    FillColorProperty.cpp
    FillColorProperty.h
    FillZonePropFactory.cpp
    FillZonePropFactory.h
    FillZoneComparator.cpp
    FillZoneComparator.h
    FillZoneEditor.cpp
    FillZoneEditor.h
    ColorPickupInteraction.cpp
    ColorPickupInteraction.h
    DespeckleState.cpp
    DespeckleState.h
    DespeckleView.cpp
    DespeckleView.h
    DespeckleVisualization.cpp
    DespeckleVisualization.h
    DespeckleLevel.cpp
    DespeckleLevel.h
    DewarpingView.cpp
    DewarpingView.h
    DewarpingMode.cpp
    DewarpingMode.h
    ChangeDewarpingDialog.cpp
    ChangeDewarpingDialog.h
    DepthPerception.cpp
    DepthPerception.h
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

#===============================================================================
# Translation Sources
#===============================================================================

TRANSLATION_SOURCES(scantailor ${SOURCES} ${UI_FILES})