#===============================================================================
# Scan Tailor - Modernized Build Configuration
# Requires: CMake 3.16+, C++23 compatible compiler, Qt5
#===============================================================================

CMAKE_MINIMUM_REQUIRED(VERSION 3.16)

#===============================================================================
# CMAKE POLICIES AND OVERRIDES
#===============================================================================
IF(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Set custom compiler flags
SET(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_SOURCE_DIR}/cmake/default_cflags.cmake")
SET(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX "${CMAKE_SOURCE_DIR}/cmake/default_cxxflags.cmake")

#===============================================================================
# PROJECT CONFIGURATION
#===============================================================================
PROJECT("Scan Tailor" C CXX)

# Modern C++ Standard (C++23)
SET(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)
ADD_COMPILE_OPTIONS(-fPIC)
ADD_COMPILE_OPTIONS(-Werror)
ADD_COMPILE_OPTIONS(-Wno-deprecated-declarations)

# Platform-specific definitions
ADD_DEFINITIONS(-D_BIND_TO_CURRENT_VCLIBS_VERSION=1)

# Optional debug CLI
IF(DEBUG_CLI)
	ADD_DEFINITIONS(-DDEBUG_CLI)
ENDIF(DEBUG_CLI)

#===============================================================================
# VERSION EXTRACTION
#===============================================================================
# Copy version.h to build directory to avoid conflicts
CONFIGURE_FILE(
	"${PROJECT_SOURCE_DIR}/src/version.h"
	"${PROJECT_BINARY_DIR}/.version.h" COPYONLY
)

# Remove leftover from previous builds
IF(NOT "${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
	FILE(REMOVE "${PROJECT_BINARY_DIR}/version.h")
ENDIF()

# Extract VERSION from version.h
FILE(READ "${PROJECT_SOURCE_DIR}/src/version.h" version_h_contents)
STRING(
	REGEX REPLACE
	".*#define[ \\t]+VERSION[ \\t]+\"([^\"]*)\".*"
	"\\1" VERSION "${version_h_contents}"
)
IF("${VERSION}" STREQUAL "${version_h_contents}")
	MESSAGE(FATAL_ERROR "Failed to extract VERSION from version.h")
ENDIF()

# Extract VERSION_QUAD from version.h
STRING(
	REGEX REPLACE
	".*#define[ \\t]+VERSION_QUAD[ \\t]+\"(([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)?)\".*"
	"\\1" VERSION_QUAD "${version_h_contents}"
)
IF("${VERSION_QUAD}" STREQUAL "${version_h_contents}")
	MESSAGE(FATAL_ERROR "Failed to extract VERSION_QUAD from version.h")
ENDIF()

#===============================================================================
# QT5 CONFIGURATION
#===============================================================================
FIND_PACKAGE(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED)
SET(CMAKE_AUTOMOC OFF)
SET(CMAKE_AUTORCC OFF)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOUIC_SEARCH_PATHS 
	"${CMAKE_SOURCE_DIR}/ui"
	"${CMAKE_SOURCE_DIR}/filters/deskew/ui"
	"${CMAKE_SOURCE_DIR}/filters/fix_orientation/ui"
	"${CMAKE_SOURCE_DIR}/filters/output/ui"
	"${CMAKE_SOURCE_DIR}/filters/page_layout/ui"
	"${CMAKE_SOURCE_DIR}/filters/page_split/ui"
	"${CMAKE_SOURCE_DIR}/filters/select_content/ui"
	"${CMAKE_SOURCE_DIR}/crash_reporter/ui")

# Include Qt5 headers
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})

#===============================================================================
# INCLUDE DIRECTORIES (PRIMARY)
#===============================================================================
INCLUDE_DIRECTORIES(. src foundation math interaction zones)

#===============================================================================
# CMAKE MODULES AND HELPERS
#===============================================================================
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(TestCXXAcceptsFlag)
INCLUDE(cmake/SetDefaultBuildType.cmake)
INCLUDE(cmake/SetDefaultGccFlags.cmake)
INCLUDE(cmake/ToNativePath.cmake)
INCLUDE(cmake/UpdateTranslations.cmake)
INCLUDE(cmake/CopyToBuildDir.cmake)
INCLUDE(cmake/LibToDLL.cmake)

# Set default build type and apply GCC-specific flags if applicable
ST_SET_DEFAULT_BUILD_TYPE(Release)
IF(CMAKE_COMPILER_IS_GNUCC)
	ST_SET_DEFAULT_GCC_FLAGS()
ENDIF(CMAKE_COMPILER_IS_GNUCC)

#===============================================================================
# DIRECTORY PATHS
#===============================================================================
GET_FILENAME_COMPONENT(source_outer_dir "${PROJECT_SOURCE_DIR}/../../../.." ABSOLUTE)
GET_FILENAME_COMPONENT(build_outer_dir "${PROJECT_BINARY_DIR}/.." ABSOLUTE)
SET(STAGING_LIBS_DIR "")

# Windows-specific: Find staging directory for dependencies
IF(WIN32)
	FIND_PATH(
		DEPS_BUILD_DIR build-qt.bat
		HINTS "${build_outer_dir}/scantailor-deps-build"
		DOC "Build directory for Scan Tailor dependencies."
	)
	IF(NOT DEPS_BUILD_DIR)
		MESSAGE(
			FATAL_ERROR "The build directory for Scan Tailor dependencies could not be found! "
			"You can specify it manually in DEPS_BUILD_DIR variable."
		)
	ENDIF()
	SET(STAGING_LIBS_DIR "${DEPS_BUILD_DIR}/staging/libs")
ENDIF()

#===============================================================================
# EXTERNAL PACKAGES AND LIBRARIES
#===============================================================================

# ===== Qt5 Framework =====
FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Gui Xml Widgets)
IF(NOT Qt5_FOUND)
	MESSAGE(
		FATAL_ERROR "Qt5 could not be found. Please install Qt5 development packages."
	)
ENDIF()
GET_FILENAME_COMPONENT(QT_BIN_DIR "${Qt5_DIR}/../../../bin" ABSOLUTE)

# ===== OpenGL Support (Optional) =====
INCLUDE(FindOpenGL)
SET(use_opengl OFF)
IF(OPENGL_FOUND)
	# Disabled for now due to various compatibility issues
	# SET(use_opengl ON)
ENDIF()
OPTION(ENABLE_OPENGL "OpenGL may be used for UI acceleration" ${use_opengl})

#===============================================================================
# EXTERNAL LIBRARY DETECTION - IMAGE FORMATS
#===============================================================================

# Locate directories for image libraries
FILE(GLOB jpeg_dirs1 "${build_outer_dir}/jpeg-[0-9]*")
FILE(GLOB jpeg_dirs2 "${source_outer_dir}/jpeg-[0-9]*")
FILE(GLOB zlib_dirs1 "${build_outer_dir}/zlib-[0-9]*.[0-9]*.[0-9]*")
FILE(GLOB zlib_dirs2 "${source_outer_dir}/zlib-[0-9]*.[0-9]*.[0-9]*")
FILE(GLOB png_dirs1 "${build_outer_dir}/libpng-[0-9]*.[0-9]*.[0-9]*")
FILE(GLOB png_dirs2 "${source_outer_dir}/libpng-[0-9]*.[0-9]*.[0-9]*")
FILE(GLOB png_dirs3 "${build_outer_dir}/lpng[0-9]*")
FILE(GLOB png_dirs4 "${source_outer_dir}/lpng[0-9]*")
FILE(GLOB tiff_dirs1 "${build_outer_dir}/tiff-[0-9]*.[0-9]*.[0-9]*")
FILE(GLOB tiff_dirs2 "${source_outer_dir}/tiff-[0-9]*.[0-9]*.[0-9]*")

# ===== JPEG Library =====
FIND_PATH(
	JPEG_INCLUDE_DIR jpeglib.h
	PATHS /usr/local/include /usr/include
	HINTS ${jpeg_dirs1} ${jpeg_dirs2}
	DOC "Path to libjpeg headers."
)
IF(NOT JPEG_INCLUDE_DIR)
	MESSAGE(FATAL_ERROR "Could not find jpeg headers. Install libjpeg-dev or similar.")
ENDIF()
INCLUDE_DIRECTORIES("${JPEG_INCLUDE_DIR}")

FIND_LIBRARY(
	JPEG_LIBRARY NAMES jpeg libjpeg.lib
	PATHS /usr/local/lib /usr/lib
	HINTS ${jpeg_dirs1} ${jpeg_dirs2} ${STAGING_LIBS_DIR}
	DOC "Path to jpeg library."
)
IF(NOT JPEG_LIBRARY)
	MESSAGE(FATAL_ERROR "Could not find jpeg library. Install libjpeg-dev or similar.")
ENDIF()

# ===== Zlib Library =====
FIND_PATH(
	ZLIB_INCLUDE_DIR zlib.h
	PATHS /usr/local/include /usr/include
	HINTS ${zlib_dirs1} ${zlib_dirs2}
	DOC "Path to zlib headers."
)
IF(NOT ZLIB_INCLUDE_DIR)
	MESSAGE(FATAL_ERROR "Could not find zlib headers. Install zlib1g-dev or similar.")
ENDIF()
INCLUDE_DIRECTORIES("${ZLIB_INCLUDE_DIR}")

FIND_LIBRARY(
	ZLIB_LIBRARY NAMES z zdll.lib
	PATHS /usr/local/lib /usr/lib
	HINTS ${zlib_dirs1} ${zlib_dirs2} ${STAGING_LIBS_DIR}
	DOC "Path to zlib library."
)
IF(NOT ZLIB_LIBRARY)
	MESSAGE(FATAL_ERROR "Could not find zlib library. Install zlib1g-dev or similar.")
ENDIF()

# ===== PNG Library =====
FIND_PATH(
	PNG_INCLUDE_DIR png.h
	PATHS /usr/local/include /usr/include
	HINTS ${png_dirs1} ${png_dirs2} ${png_dirs3} ${png_dirs4}
	DOC "Path to libpng headers."
)
IF(NOT PNG_INCLUDE_DIR)
	MESSAGE(FATAL_ERROR "Could not find libpng headers. Install libpng-dev or similar.")
ENDIF()
INCLUDE_DIRECTORIES("${PNG_INCLUDE_DIR}")

FIND_LIBRARY(
	PNG_LIBRARY NAMES png libpng.lib
	PATHS /usr/local/lib /usr/lib
	HINTS ${png_dirs1} ${png_dirs2} ${png_dirs3} ${png_dirs4} ${STAGING_LIBS_DIR}
	DOC "Path to png library."
)
IF(NOT PNG_LIBRARY)
	MESSAGE(FATAL_ERROR "Could not find libpng library. Install libpng-dev or similar.")
ENDIF()

# ===== TIFF Library =====
FIND_PATH(
	TIFF_INCLUDE_DIR tiff.h
	PATHS /usr/local/include /usr/include
	HINTS ${tiff_dirs1} ${tiff_dirs2}
	PATH_SUFFIXES libtiff
	DOC "Path to libtiff headers."
)
IF(NOT TIFF_INCLUDE_DIR)
	MESSAGE(FATAL_ERROR "Could not find libtiff headers. Install libtiff-dev or similar.")
ENDIF()
INCLUDE_DIRECTORIES("${TIFF_INCLUDE_DIR}")

FIND_LIBRARY(
	TIFF_LIBRARY tiff libtiff.lib
	PATHS /usr/local/lib /usr/lib
	HINTS ${tiff_dirs1} ${tiff_dirs2} ${STAGING_LIBS_DIR}
	PATH_SUFFIXES libtiff
	DOC "Path to tiff library."
)
IF(NOT TIFF_LIBRARY)
	MESSAGE(FATAL_ERROR "Could not find libtiff library. Install libtiff-dev or similar.")
ENDIF()

# Platform-specific TIFF configuration
IF(WIN32)
	ADD_DEFINITIONS(-DUSE_LIBTIFF_DLL)
ENDIF()

#===============================================================================
# EXTRA LIBRARIES AND PLATFORM-SPECIFIC SETTINGS
#===============================================================================

SET(EXTRA_LIBS "")

# UNIX/Linux specific requirements
IF(UNIX)
	# XRender support for X11
	INCLUDE(FindX11)
	IF(NOT X11_Xrender_FOUND)
		MESSAGE(
			FATAL_ERROR "Could not find Xrender. Install libxrender-dev or similar."
		)
	ENDIF()
	INCLUDE_DIRECTORIES("${X11_Xrender_INCLUDE_PATH}")
	LIST(APPEND EXTRA_LIBS ${X11_Xrender_LIB})
ELSEIF(WIN32)
	ADD_DEFINITIONS(-DNOMINMAX)
ENDIF(UNIX)

# Check for stdint.h availability
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
IF(NOT HAVE_STDINT_H)
	CONFIGURE_FILE(compat/pstdint.h "${CMAKE_CURRENT_BINARY_DIR}/stdint.h" COPYONLY)
	INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
ELSE(NOT HAVE_STDINT_H)
	FILE(REMOVE "${CMAKE_CURRENT_BINARY_DIR}/stdint.h")
ENDIF(NOT HAVE_STDINT_H)

# Windows-specific libraries
IF(WIN32)
	LIST(
		APPEND EXTRA_LIBS
		winmm imm32 ws2_32 ole32 oleaut32 uuid gdi32 comdlg32 winspool
	)
ENDIF()

# Aggregate extra libraries
LIST(APPEND EXTRA_LIBS ${TIFF_LIBRARY} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${JPEG_LIBRARY})

IF(ENABLE_OPENGL)
	LIST(APPEND EXTRA_LIBS ${OPENGL_LIBRARIES})
ENDIF()

#===============================================================================
# CONFIGURATION OPTIONS
#===============================================================================

# Crash reporter (Windows + MSVC only)
IF(MSVC)
	OPTION(ENABLE_CRASH_REPORTER "Enable crash reporter (official builds only)" OFF)
ENDIF(MSVC)

# Translation directories
IF(WIN32)
	SET(TRANSLATIONS_DIR_REL "translations")
ELSE(WIN32)
	SET(TRANSLATIONS_DIR_REL "share/scantailor/translations")
ENDIF(WIN32)
SET(TRANSLATIONS_DIR_ABS "${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS_DIR_REL}")

# Generate config.h from template
CONFIGURE_FILE(config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)

#===============================================================================
# SUBDIRECTORIES
#===============================================================================

# Core libraries
ADD_SUBDIRECTORY(crash_reporter)
ADD_SUBDIRECTORY(dewarping)
ADD_SUBDIRECTORY(foundation)
ADD_SUBDIRECTORY(math)
ADD_SUBDIRECTORY(imageproc)
ADD_SUBDIRECTORY(interaction)
ADD_SUBDIRECTORY(zones)

#===============================================================================
# UI AND RESOURCE FILES
#===============================================================================

# Separate common and GUI-only UI files
FILE(GLOB common_ui_files ui/ErrorWidget.ui)
FILE(GLOB gui_only_ui_files "ui/*.ui")
FOREACH(ui_file ${common_ui_files})
	LIST(REMOVE_ITEM gui_only_ui_files "${ui_file}")
ENDFOREACH()

SOURCE_GROUP("UI Files" FILES ${common_ui_files} ${gui_only_ui_files})
QT5_WRAP_UI(common_ui_sources ${common_ui_files})
QT5_WRAP_UI(gui_only_ui_sources ${gui_only_ui_files})
SET_SOURCE_FILES_PROPERTIES(${common_ui_sources} ${gui_only_ui_files} PROPERTIES GENERATED TRUE)

ADD_SUBDIRECTORY(ui)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")
ADD_CUSTOM_TARGET(toplevel_ui_sources DEPENDS ${common_ui_sources} ${gui_only_ui_sources})

# Filter modules (image processing filters)
ADD_SUBDIRECTORY(filters/fix_orientation)
ADD_SUBDIRECTORY(filters/page_split)
ADD_SUBDIRECTORY(filters/deskew)
ADD_SUBDIRECTORY(filters/select_content)
ADD_SUBDIRECTORY(filters/page_layout)
ADD_SUBDIRECTORY(filters/output)

# Qt resource files
SET(resource_sources)
QT5_ADD_RESOURCES(resource_sources resources/resources.qrc)
SOURCE_GROUP("Generated" FILES ${common_ui_sources} ${gui_only_ui_sources} ${resource_sources})
SOURCE_GROUP("Resources" FILES resources/resources.qrc resources/icons/COPYING)
IF(WIN32)
	SOURCE_GROUP("Resources" FILES resources/win32/resources.rc)
ENDIF()

#===============================================================================
# SOURCE FILES
#===============================================================================

# Common sources (shared between GUI and CLI)
SET(
	common_sources
	src/BackgroundExecutor.cpp src/BackgroundExecutor.h
	src/OpenGLSupport.cpp src/OpenGLSupport.h
	src/PixmapRenderer.cpp src/PixmapRenderer.h
	src/BubbleAnimation.cpp src/BubbleAnimation.h
	src/ProcessingIndicationWidget.cpp src/ProcessingIndicationWidget.h
	src/NonOwningWidget.cpp src/NonOwningWidget.h
	src/Dpi.cpp src/Dpi.h src/Dpm.cpp src/Dpm.h
	src/SmartFilenameOrdering.cpp src/SmartFilenameOrdering.h
	src/AbstractRelinker.h
	src/RelinkablePath.cpp src/RelinkablePath.h
	src/ImageInfo.cpp src/ImageInfo.h
	src/ImageFileInfo.cpp src/ImageFileInfo.h
	src/ImageMetadata.cpp src/ImageMetadata.h
	src/RecentProjects.cpp src/RecentProjects.h
	src/OutOfMemoryHandler.cpp src/OutOfMemoryHandler.h
	src/CommandLine.cpp src/CommandLine.h
	src/PageSelectionAccessor.cpp src/PageSelectionAccessor.h
	src/PageSelectionProvider.h
	src/ContentSpanFinder.cpp src/ContentSpanFinder.h
	src/PhysicalTransformation.cpp src/PhysicalTransformation.h
	src/ImageTransformation.cpp src/ImageTransformation.h
	src/ImagePixmapUnion.h
	src/ImageViewBase.cpp src/ImageViewBase.h
	src/BasicImageView.cpp src/BasicImageView.h
	src/DebugImageView.cpp src/DebugImageView.h
	src/TabbedDebugImages.cpp src/TabbedDebugImages.h
	src/ThumbnailLoadResult.h
	src/ThumbnailPixmapCache.cpp src/ThumbnailPixmapCache.h
	src/ThumbnailBase.cpp src/ThumbnailBase.h
	src/ThumbnailFactory.cpp src/ThumbnailFactory.h
	src/IncompleteThumbnail.cpp src/IncompleteThumbnail.h
	src/ContentBoxPropagator.cpp src/ContentBoxPropagator.h
	src/PageOrientationPropagator.cpp src/PageOrientationPropagator.h
	src/DebugImages.cpp src/DebugImages.h
	src/ImageId.cpp src/ImageId.h
	src/PageId.cpp src/PageId.h
	src/PageInfo.cpp src/PageInfo.h
	src/BackgroundTask.cpp src/BackgroundTask.h
	src/ProcessingTaskQueue.cpp src/ProcessingTaskQueue.h
	src/PageSequence.cpp src/PageSequence.h
	src/StageSequence.cpp src/StageSequence.h
	src/ProjectPages.cpp src/ProjectPages.h
	src/FilterData.cpp src/FilterData.h
	src/ImageMetadataLoader.cpp src/ImageMetadataLoader.h
	src/TiffReader.cpp src/TiffReader.h
	src/TiffWriter.cpp src/TiffWriter.h
	src/PngMetadataLoader.cpp src/PngMetadataLoader.h
	src/TiffMetadataLoader.cpp src/TiffMetadataLoader.h
	src/JpegMetadataLoader.cpp src/JpegMetadataLoader.h
	src/ImageLoader.cpp src/ImageLoader.h
	src/ErrorWidget.cpp src/ErrorWidget.h
	src/OrthogonalRotation.cpp src/OrthogonalRotation.h
	src/WorkerThread.cpp src/WorkerThread.h
	src/LoadFileTask.cpp src/LoadFileTask.h
	src/FilterOptionsWidget.cpp src/FilterOptionsWidget.h
	src/TaskStatus.h src/FilterUiInterface.h
	src/ProjectReader.cpp src/ProjectReader.h
	src/ProjectWriter.cpp src/ProjectWriter.h
	src/XmlMarshaller.cpp src/XmlMarshaller.h
	src/XmlUnmarshaller.cpp src/XmlUnmarshaller.h
	src/AtomicFileOverwriter.cpp src/AtomicFileOverwriter.h
	src/EstimateBackground.cpp src/EstimateBackground.h
	src/Despeckle.cpp src/Despeckle.h
	src/ThreadPriority.cpp src/ThreadPriority.h
	src/FileNameDisambiguator.cpp src/FileNameDisambiguator.h
	src/OutputFileNameGenerator.cpp src/OutputFileNameGenerator.h
	src/PageRange.cpp src/PageRange.h
	src/SelectedPage.cpp src/SelectedPage.h
	src/Utils.cpp src/Utils.h
	src/PageView.h
	src/AutoManualMode.h
	src/AbstractCommand.h
	src/AbstractFilter.h
	src/BeforeOrAfter.h
	src/FilterResult.h
	src/CompositeCacheDrivenTask.h
	src/Margins.h
	src/ChangedStateItemDelegate.h
	src/PageOrderProvider.h
	src/PageOrderOption.h
	src/PayloadEvent.h
	filter_dc/AbstractFilterDataCollector.h
	filter_dc/ThumbnailCollector.h
	filter_dc/ContentBoxCollector.h
	filter_dc/PageOrientationCollector.h
	src/version.h
	config.h.in
	${common_ui_files}
)

# GUI-only sources
SET(gui_only_sources
	src/Application.cpp src/Application.h
	src/SkinnedButton.cpp src/SkinnedButton.h
	src/StageListView.cpp src/StageListView.h
	src/RelinkablePathVisualization.cpp src/RelinkablePathVisualization.h
	src/RelinkingModel.cpp src/RelinkingModel.h
	src/RelinkingSortingModel.cpp src/RelinkingSortingModel.h
	src/RelinkingListView.cpp src/RelinkingListView.h
	src/RelinkingDialog.cpp src/RelinkingDialog.h
	src/SettingsDialog.cpp src/SettingsDialog.h
	src/FixDpiDialog.cpp src/FixDpiDialog.h
	src/LoadFilesStatusDialog.cpp src/LoadFilesStatusDialog.h
	src/ProjectCreationContext.cpp src/ProjectCreationContext.h
	src/ProjectOpeningContext.cpp src/ProjectOpeningContext.h
	src/OutOfMemoryDialog.cpp src/OutOfMemoryDialog.h
	src/ThumbnailSequence.cpp src/ThumbnailSequence.h
	src/ProjectFilesDialog.cpp src/ProjectFilesDialog.h
	src/NewOpenProjectPanel.cpp src/NewOpenProjectPanel.h
	src/SystemLoadWidget.cpp src/SystemLoadWidget.h
	src/MainWindow.cpp src/MainWindow.h
	src/main.cpp
)

# CLI-only sources
SET(cli_only_sources
	src/ConsoleBatch.cpp src/ConsoleBatch.h
	src/main-cli.cpp
)

SOURCE_GROUP("Sources" FILES ${common_sources} ${gui_only_sources} ${cli_only_sources})
SOURCE_GROUP("Special Headers" FILES version.h config.h.in)

#===============================================================================
# Qt5 AUTOMATIC MOC/RCC/UIC
#===============================================================================
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

#===============================================================================
# COMPILER WORKAROUNDS
#===============================================================================
IF(CMAKE_COMPILER_IS_GNUCXX)
	# Workaround for GCC optimization issue with page sorting
	SET_SOURCE_FILES_PROPERTIES(
		ThumbnailSequence.cpp PROPERTIES
		COMPILE_FLAGS "-fno-strict-aliasing"
	)
ENDIF()

#===============================================================================
# WINDOWS RESOURCE FILE
#===============================================================================
SET(win32_resource_file)
IF(WIN32)
	SET(rc_file "${CMAKE_SOURCE_DIR}/resources/win32/resources.rc")
	FILE(GLOB win32_resources resources/win32/*.ico)
	SET_SOURCE_FILES_PROPERTIES(
		"${rc_file}" PROPERTIES
		OBJECT_DEPENDS ${win32_resources}
	)
	IF(MINGW)
		SET(win32_resource_file "${CMAKE_BINARY_DIR}/win32_resources.o")
		ADD_CUSTOM_COMMAND(
			OUTPUT "${win32_resource_file}"
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/resources/win32"
			COMMAND windres -i "${rc_file}" -o "${win32_resource_file}"
			MAIN_DEPENDENCY "${rc_file}"
			DEPENDS ${win32_resources}
		)
	ELSE(MINGW)
		SET(win32_resource_file "${rc_file}")
	ENDIF(MINGW)
ENDIF(WIN32)

#===============================================================================
# BUILD TARGETS
#===============================================================================

# Core library (common code)
ADD_LIBRARY(stcore STATIC ${common_sources} ${common_ui_sources})
TARGET_LINK_LIBRARIES(stcore Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml)

# GUI application
ADD_EXECUTABLE(
	scantailor WIN32 
	${gui_only_sources} ${common_ui_sources} ${gui_only_ui_sources}
	${resource_sources} ${win32_resource_file} resources/icons/COPYING
)

# CLI application
ADD_EXECUTABLE(scantailor-cli ${cli_only_sources} ${common_ui_sources})

#===============================================================================
# LINK LIBRARIES
#===============================================================================

# GUI application dependencies
TARGET_LINK_LIBRARIES(
	scantailor
	fix_orientation page_split deskew select_content page_layout output
	stcore dewarping zones interaction imageproc math foundation
	Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml
	${EXTRA_LIBS}
)

# CLI application dependencies
TARGET_LINK_LIBRARIES(
	scantailor-cli
	fix_orientation page_split deskew select_content page_layout output
	stcore dewarping zones interaction imageproc math foundation
	Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml
	${EXTRA_LIBS}
)

#===============================================================================
# INSTALLATION
#===============================================================================
INSTALL(TARGETS scantailor scantailor-cli RUNTIME DESTINATION bin)

#===============================================================================
# CRASH REPORTER (Windows/MSVC only)
#===============================================================================
IF(ENABLE_CRASH_REPORTER)
	FIND_PATH(
		SYMBOLS_PATH . PATHS "${build_outer_dir}/symbols" "${source_outer_dir}/symbols"
		NO_DEFAULT_PATH
		DOC "The directory to write symbol information into."
	)
	IF(NOT SYMBOLS_PATH)
		MESSAGE(FATAL_ERROR "SYMBOLS_PATH directory is not set.")
	ENDIF(NOT SYMBOLS_PATH)

	FIND_PROGRAM(
		DUMP_SYMS_EXECUTABLE dump_syms
		PATHS "${build_outer_dir}" "${source_outer_dir}"
		DOC "Path to dump_syms.exe (Mozilla repository)"
	)
	IF(NOT DUMP_SYMS_EXECUTABLE)
		MESSAGE(FATAL_ERROR "dump_syms.exe not found. Specify location with DUMP_SYMS_EXECUTABLE.")
	ENDIF(NOT DUMP_SYMS_EXECUTABLE)

	INCLUDE_DIRECTORIES(crash_reporter crash_reporter/google-breakpad)
	TARGET_LINK_LIBRARIES(scantailor crash_handler)
ENDIF(ENABLE_CRASH_REPORTER)

#===============================================================================
# TRANSLATIONS (Currently disabled - needs Qt5 compatible implementation)
#===============================================================================
TRANSLATION_SOURCES(scantailor ${sources} ${ui_files})
FILE(GLOB TRANSLATION_FILES translations/scantailor_*.ts)
FILE(GLOB CR_TRANSLATION_FILES translations/crashreporter_*.ts)

FINALIZE_TRANSLATION_SET(scantailor ${TRANSLATION_FILES})
FINALIZE_TRANSLATION_SET(crashreporter ${CR_TRANSLATION_FILES})
UPDATE_TRANSLATIONS_TARGET(update_translations scantailor crashreporter)

SET(ts_files ${TRANSLATION_FILES})
IF(ENABLE_CRASH_REPORTER)
	LIST(APPEND ts_files ${CR_TRANSLATION_FILES})
ENDIF(ENABLE_CRASH_REPORTER)

# Filter out untranslated files
SET(FILTERED_TRANSLATION_FILES)
FOREACH(ts_file ${ts_files})
	IF("${ts_file}" MATCHES ".*untranslated.ts")
		# Skip untranslated files
	ELSE("${ts_file}" MATCHES ".*untranslated.ts")
		LIST(APPEND FILTERED_TRANSLATION_FILES "${ts_file}")
	ENDIF("${ts_file}" MATCHES ".*untranslated.ts")
ENDFOREACH(ts_file)

# TODO: Re-enable QT5_CREATE_TRANSLATION when Qt5 translation support is fully implemented
# QT5_CREATE_TRANSLATION(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${FILTERED_TRANSLATION_FILES})
# ADD_CUSTOM_TARGET(compile_translations ALL DEPENDS ${QM_FILES})
# INSTALL(FILES ${QM_FILES} DESTINATION "${TRANSLATIONS_DIR_REL}/")

#===============================================================================
# SOURCE DISTRIBUTION PACKAGING
#===============================================================================
SET(CPACK_CMAKE_GENERATOR "")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "scantailor-${VERSION}")
SET(
	CPACK_SOURCE_IGNORE_FILES
	"/\\\\.svn/"
	"/\\\\.git/"
	"~$"
	"\\\\.pcs$"
	"TODO.txt"
	"CMakeLists.txt.user"
	"/doxygen/"
	"${CMAKE_BINARY_DIR}"
)
INCLUDE(CPack)

#===============================================================================
# WINDOWS-SPECIFIC DLL DEPLOYMENT
#===============================================================================
IF(WIN32)
	IF(MINGW)
		CHECK_CXX_ACCEPTS_FLAG("-m64" MINGW64)
		IF(NOT MINGW64)
			FIND_FILE(
				MINGWM10_DLL mingwm10.dll
				PATHS c:/MinGW/bin ENV PATH
				DOC "Full path to mingwm10.dll"
			)
			IF(NOT MINGWM10_DLL)
				MESSAGE(FATAL_ERROR "Could not find mingwm10.dll")
			ENDIF(NOT MINGWM10_DLL)
			COPY_TO_BUILD_DIR("${MINGWM10_DLL}")

			FIND_FILE(
				GCC_DW2_DLL libgcc_s_dw2-1.dll
				PATHS c:/MinGW/bin ENV PATH
				DOC "Full path to libgcc_s_dw2-1.dll"
			)
			IF(NOT GCC_DW2_DLL)
				MESSAGE("Could not find libgcc_s_dw2-1.dll (may be normal for recent MinGW)")
			ELSE()
				COPY_TO_BUILD_DIR("${GCC_DW2_DLL}")
			ENDIF()
		ENDIF(NOT MINGW64)
	ENDIF(MINGW)

	# Qt5 library DLL copying (Debug configuration)
	SET(
		libs_copy_debug
		"${QT_QTCORE_LIBRARY_DEBUG}"
		"${QT_QTGUI_LIBRARY_DEBUG}"
		"${QT_QTXML_LIBRARY_DEBUG}"
		"${QT_QTNETWORK_LIBRARY_DEBUG}"
	)
	LIB_TO_DLL(libs_copy_debug ${libs_copy_debug})
	COPY_TO_BUILD_DIR(${libs_copy_debug} CONFIGURATIONS Debug)

	# Qt5 library DLL copying (Release configuration)
	SET(
		libs_copy_nondebug
		"${QT_QTCORE_LIBRARY_RELEASE}"
		"${QT_QTGUI_LIBRARY_RELEASE}"
		"${QT_QTXML_LIBRARY_RELEASE}"
		"${QT_QTNETWORK_LIBRARY_RELEASE}"
	)
	LIB_TO_DLL(libs_copy_nondebug ${libs_copy_nondebug})
	COPY_TO_BUILD_DIR(${libs_copy_nondebug} CONFIGURATIONS Release MinSizeRel RelWithDebInfo)

	# Plugin copying
	COPY_TO_BUILD_DIR("${QT_QJPEG_PLUGIN_RELEASE}" SUBDIR imageformats CONFIGURATIONS Release MinSizeRel RelWithDebInfo)
	COPY_TO_BUILD_DIR("${QT_QJPEG_PLUGIN_DEBUG}" SUBDIR imageformats CONFIGURATIONS Debug)

	# Dependencies DLL copying
	IF(MINGW)
		SET(zlib_dll "${QT_BIN_DIR}/libz.dll")
	ELSE()
		SET(zlib_dll "${QT_BIN_DIR}/zdll.dll")
	ENDIF()
	SET(
		libs_copy_all_confs
		"${QT_BIN_DIR}/libpng.dll" "${QT_BIN_DIR}/libjpeg.dll"
		"${QT_BIN_DIR}/libtiff.dll" "${zlib_dll}"
	)
	COPY_TO_BUILD_DIR(${libs_copy_all_confs})

	GENERATE_COPY_TO_BUILD_DIR_TARGET(copy_to_build_dir)
	ADD_SUBDIRECTORY(packaging/windows)
ENDIF(WIN32)
