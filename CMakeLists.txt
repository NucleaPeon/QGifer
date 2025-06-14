CMAKE_MINIMUM_REQUIRED (VERSION 3.10)

SET(QGIFER_VERSION_RELEASE 0)
SET(QGIFER_VERSION_MAJOR 3)
SET(QGIFER_VERSION_MINOR 0)
SET(QGIFER_VERSION_PATCH 0)

project(QGifer
        VERSION ${QGIFER_VERSION_RELEASE}.${QGIFER_VERSION_MAJOR}.${QGIFER_VERSION_MINOR}.${QGIFER_VERSION_PATCH}
        LANGUAGES C CXX
        DESCRIPTION "Gif Editor Application"
)

include(GNUInstallDirs)
include(CTest)

# Immediately set the version.h header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h @ONLY)

if (APPLE)
    set(CMAKE_C_COMPILER /usr/bin/clang)
    set(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif()

set(TARGET_NAME ${PROJECT_NAME})

#------------------------------------------------------
#---------------------- Qt5 ---------------------------
#------------------------------------------------------

find_package(Qt5 REQUIRED
    Core
    Gui
    Widgets
    Test
)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

message(STATUS "${PROJECT_NAME} is the project name")


#------------------------------------------------------
#-------------------- GENERAL -------------------------
#------------------------------------------------------



# choices between "a", "b", "rc" and empty string for release
SET(QGIFER_VERSION_PRERELEASE_TAG rc)
SET(QGIFER_VERSION_PRERELEASE_VERSION 2)

IF(QGIFER_VERSION_PRERELEASE_TAG)
    SET(QGIFER_VERSION "${QGIFER_VERSION}-${QGIFER_VERSION_PRERELEASE_TAG}${QGIFER_VERSION_PRERELEASE_VERSION}")
ENDIF(QGIFER_VERSION_PRERELEASE_TAG)

OPTION(QUIET_MODE "quiet mode" OFF)
OPTION(RELEASE_MODE "release mode" OFF)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")


IF(UNIX)
    SET(CMAKE_INSTALL_PREFIX "/usr/")
	SET(BIN_INSTALL_DIR "bin")
	SET(DATA_INSTALL_DIR "share/qgifer")
	SET(ICON_INSTALL_DIR "share/icons")
	SET(DESKTOP_INSTALL_DIR "share/applications")
	SET(MENU_INSTALL_DIR "share/menu")
	SET(DOC_INSTALL_DIR "share/doc/qgifer")
ENDIF(UNIX)


SET(W32_EXEC )
IF(WIN32)
	SET(W32_EXEC WIN32)
	SET(OpenCV_DIR "C:/opencv/build")
	SET(GIF_LIBRARY "C:/Program Files/GnuWin32/lib")
	SET(GIF_INCLUDE_DIR "C:/Program Files/GnuWin32/include")
	INCLUDE_DIRECTORIES(SYSTEM "C:/Program Files/GnuWin32/include")
	SET(CMAKE_INSTALL_PREFIX "${TARGET_NAME}")
ENDIF(WIN32)

FIND_PACKAGE(OpenCV REQUIRED core highgui imgproc videoio)
FIND_PACKAGE(GIF REQUIRED)

include_directories(BEFORE res)
include_directories(BEFORE src)
include_directories(BEFORE ui)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wno-reorder -Wno-sign-compare" )

SET(WINDOWS_EXEC )

IF( QUIET_MODE )
    SET(WINDOWS_EXEC ${W32_EXEC})
    MESSAGE("Quiet mode is ON, no debug output will be shown")
    ADD_DEFINITIONS(-DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT)
ENDIF( QUIET_MODE )

IF( RELEASE_MODE )
    MESSAGE("Release mode is ON")
    ADD_DEFINITIONS(-DQT_NO_DEBUG)
    SET(CMAKE_BUILD_TYPE Release)
ELSE( RELEASE_MODE )
      MESSAGE("Release mode is OFF")
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF( RELEASE_MODE )

# uninstall target
configure_file(
    "cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)


#------------------------------------------------------
#-------------------- APP BUILD -----------------------
#------------------------------------------------------

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.h"
  )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}
	src
	src/widgets
        src/utils
	ui
)

list(APPEND HEADERS
        version.h
        src/gifcreator.h
        src/textrenderer.h
        src/utils/qgiferformatter.h
)

list(APPEND QT_HEADERS
        src/frameplayer.h
        src/previewwidget.h
        src/mainwindow.h
        src/gifwidget.h
        src/qgifcreator.h
        src/palettewidget.h
        src/qmultislider.h
        src/optimizerdialog.h
        src/converterdialog.h
        src/aboutdialog.h
        src/workspace.h
        src/workspaceobject.h
        src/objectwidget.h
        src/textwidget.h
        src/interpolationdialog.h
        src/textobject.h

        src/widgets/QJumpSlider.h
        src/widgets/QPressAction.h


)

list(APPEND SOURCES
        src/frameplayer.cpp
        src/main.cpp
        src/previewwidget.cpp
        src/mainwindow.cpp
        src/gifwidget.cpp
        src/gifcreator.cpp
        src/qgifcreator.cpp
        src/palettewidget.cpp
        src/qmultislider.cpp
        src/optimizerdialog.cpp
        src/converterdialog.cpp
        src/aboutdialog.cpp
        src/workspace.cpp
        src/workspaceobject.cpp
        src/objectwidget.cpp
        src/textwidget.cpp
        src/textrenderer.cpp
        src/interpolationdialog.cpp
        src/textobject.cpp

        src/widgets/QJumpSlider.cpp
        src/widgets/QPressAction.cpp

        src/utils/qgiferformatter.cpp
)

list(APPEND RESOURCES
        res.qrc
)

list(APPEND FORMS
        ui/frameplayer.ui
        ui/mainwindow.ui
        ui/gifwidget.ui
        ui/optimizerdialog.ui
        ui/converterdialog.ui
        ui/aboutdialog.ui
        ui/objectwidget.ui
        ui/textwidget.ui
        ui/interpolationdialog.ui
)

FILE(GLOB LOCALES
        "${CMAKE_CURRENT_SOURCE_DIR}/locale/*.qm"
)

SET(DOCS
        doc/ChangeLog
        doc/COPYRIGHT
        doc/LICENSE
        doc/README
)
#[[
QT4_WRAP_CPP(HEADERS_MOC ${QT_HEADERS})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})]]

IF(WIN32)
	SET(OPENCV_LIBS "-lopencv_core${CVLIB_SUFFIX} -lopencv_highgui${CVLIB_SUFFIX} -lopencv_imgproc${CVLIB_SUFFIX} -lopencv_videoio${CVLIB_SUFFIX}" )
ELSE(WIN32)
	SET(OPENCV_LIBS "-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio" )
ENDIF(WIN32)

SET(GIF_LIBS "-lgif" )


set(RES_FILES "")
if(MINGW)
  set(RES_FILES "icon.rc")
  set(CMAKE_RC_COMPILER_INIT windres)
  ENABLE_LANGUAGE(RC)

  SET(GIF_LIBRARIES
  ${GIF_LIBRARIES}
  "${GIF_LIBRARIES}/libgif.dll.a"
  )
  SET(CMAKE_RC_COMPILE_OBJECT
    "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

ADD_EXECUTABLE(${TARGET_NAME} ${WINDOWS_EXEC} ${SOURCES} ${HEADERS_MOC} ${FORMS_HEADERS} ${RES_FILES} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Test)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} ${GIF_LIBRARIES})
# TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} ${OpenCV_LIBS} ${GIF_LIBRARIES})

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE .. )
target_include_directories(${PROJECT_NAME} BEFORE PRIVATE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_autogen/include/)
target_include_directories(${PROJECT_NAME} BEFORE PRIVATE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_autogen/include/${PROJECT_BINARY_DIR}/${PROJECT_NAME}_autogen/include/)

IF(UNIX)
	INSTALL(TARGETS ${TARGET_NAME} DESTINATION ${BIN_INSTALL_DIR})
	INSTALL(FILES "doc/menu/qgifer" DESTINATION ${MENU_INSTALL_DIR})
	INSTALL(FILES "doc/qgifer.desktop" DESTINATION ${DESKTOP_INSTALL_DIR})
	INSTALL(FILES "doc/qgifer.xpm" DESTINATION ${ICON_INSTALL_DIR})
        INSTALL(FILES ${LOCALES} DESTINATION ${DATA_INSTALL_DIR}/locale/)
        INSTALL(FILES ${DOCS} DESTINATION ${DOC_INSTALL_DIR})
ENDIF(UNIX)

IF(WIN32)
    SET(OpenCV_SUFFIX "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}")

    FILE(GLOB _QT_DLL_FILES
      ${QT_BINARY_DIR}/QtCore4.dll
      ${QT_BINARY_DIR}/QtGui4.dll
    )
    FILE(GLOB _OPENCV_DLL_FILES
        ${OpenCV_DIR}/bin/libopencv_core*.dll
        ${OpenCV_DIR}/bin/libopencv_highgui*.dll
        ${OpenCV_DIR}/bin/libopencv_imgproc*.dll
        ${OpenCV_DIR}/bin/opencv_ffmpeg.dll
    )
    FILE(GLOB_RECURSE _OPENCV_DLL_FILES
        ${OpenCV_DIR}/bin/libopencv_core*.dll
        ${OpenCV_DIR}/bin/libopencv_highgui*.dll
        ${OpenCV_DIR}/bin/libopencv_imgproc*.dll
    )

    FILE(GLOB_RECURSE _OPENCV_FFMPEG_DLL_FILE
        ${OpenCV_DIR}/../source/*/opencv_ffmpeg.dll
    )

    FILE(GLOB_RECURSE _GIF_DLL_FILES
        ${GIF_LIBRARY}/../*/giflib4.dll
    )
    FILE(GLOB _MINGW_DLL_FILES
        ${CMAKE_CXX_COMPILER}/../libstdc++-6.dll
        ${CMAKE_CXX_COMPILER}/../libwinpthread-1.dll
        ${CMAKE_CXX_COMPILER}/../libgcc_s_dw2-1.dll
    )

    INSTALL(TARGETS ${TARGET_NAME}
        DESTINATION .
        )

    INSTALL(FILES
        ${DOCS}
        ${_QT_DLL_FILES}
        ${_OPENCV_DLL_FILES}
        ${_GIF_DLL_FILES}
        ${_MINGW_DLL_FILES}
        DESTINATION .
        )
    INSTALL(FILES
        ${_OPENCV_FFMPEG_DLL_FILE}
        DESTINATION .
        RENAME "opencv_ffmpeg${OpenCV_SUFFIX}.dll"
        )

    INSTALL(FILES
        ${QT_QGIF_PLUGIN_RELEASE}
        ${QT_QJPEG_PLUGIN_RELEASE}
        DESTINATION imageformats/
        )

    INSTALL(FILES ${LOCALES}
        DESTINATION locale/)

ENDIF(WIN32)


INCLUDE(cmake/CMakeCPack.cmake)
