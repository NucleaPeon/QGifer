# This file is configured at cmake time, and loaded at cpack time.
# To pass variables to cpack from cmake, they must be configured
# in this file.

if(CPACK_GENERATOR MATCHES "NSIS")
  SET(CPACK_PACKAGE_FILE_NAME "qgifer_-rc2-1-setup")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY QGifer)
#  set(CPACK_NSIS_INSTALL_ROOT "")
  # set the install/unistall icon used for the installer itself
  # There is a bug in NSI that does not handle full unix paths properly.
  set(CPACK_NSIS_MUI_ICON "/home/dev/Development/QGifer/res\\icon.ico")
  set(CPACK_NSIS_MUI_UNIICON "/home/dev/Development/QGifer/res\\icon.ico")
  # set the package header icon for MUI
  set(CPACK_PACKAGE_ICON "/home/dev/Development/QGifer/res\\icon.png")
  # tell cpack to create links to the doc files
#  set(CPACK_NSIS_MENU_LINKS
#    "/html/index.html" "CMake Documentation"
#    "http://www.cmake.org" "CMake Web Site"
#    )
  # Use the icon from cmake-gui for add-remove programs
  set(CPACK_NSIS_INSTALLED_ICON_NAME "QGifer")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

  set(CPACK_NSIS_PACKAGE_NAME "QGifer")
  set(CPACK_NSIS_DISPLAY_NAME "QGifer, a video-based gif creator.")
#  set(CPACK_NSIS_HELP_LINK "http://www.cmake.org")
#  set(CPACK_NSIS_URL_INFO_ABOUT "http://www.kitware.com")
#  set(CPACK_NSIS_CONTACT Ivanov Arkadiy)
  set(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()
