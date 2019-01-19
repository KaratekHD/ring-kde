cmake_minimum_required(VERSION 3.0)

project(jamikdeintegration)

if(POLICY CMP0063)
   CMAKE_POLICY(SET CMP0063 NEW)
endif(POLICY CMP0063)

find_package(ECM 1.1.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH "${ECM_MODULE_PATH}")

find_package(KF5 QUIET COMPONENTS
    Akonadi
    AkonadiContact
    Contacts
)

include(ECMInstallIcons)
include(ECMOptionalAddSubdirectory)
include(CMakePackageConfigHelpers)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

SET(CMAKE_C_VISIBILITY_PRESET hidden)
SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
SET(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# When used with `add_subdirectory`, assume it is a static Qt plugin
get_directory_property(USES_ADD_SUBDIRECTORY PARENT_DIRECTORY)
if(USES_ADD_SUBDIRECTORY)
   set(BUILD_SHARED_LIBS OFF)
endif()

if(NOT BUILD_SHARED_LIBS)
    set(ENABLE__STATIC_PLUGIN 1)
else()
    set(ENABLE__STATIC_PLUGIN 0)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)

if(ENABLE__STATIC_PLUGIN)
    add_definitions(-DQT_PLUGIN)
    add_definitions(-DQT_STATICPLUGIN=1)
    add_definitions(-DJAMIKDEINTEGRATION_USE_STATIC_PLUGIN=1)
endif(ENABLE__STATIC_PLUGIN)

find_package(Qt5 CONFIG REQUIRED
    Core Gui Quick Widgets QuickControls2
)

set(GENERIC_LIB_VERSION "1.0.0")

# Pack the icons shared by various components
ADD_SUBDIRECTORY( sharedassets )

#File to compile
set( jamikdeintegration_LIB_SRCS
    src/colorinterface.cpp
    src/shortcutinterface.cpp
    src/actioninterface.cpp
    src/fileproverderinterface.cpp
    src/pixmapinterface.cpp
    src/serializationinterface.cpp
    src/actioncollection.cpp
    src/systray.cpp
    src/windowevent.cpp
    src/cmiconengine.cpp
    src/personiconengine.cpp
    src/plugin.cpp
)

if(Qt5TextToSpeech_FOUND)
    set(
        jamikdeintegration_LIB_SRCS
        ${jamikdeintegration_LIB_SRCS}
        accessibility.cpp
    )
endif()

if(KF5_AKONADI_FOUND AND  KF5_AKONADICONTACT_FOUND AND  KF5_CONTACTS_FOUND)
    set(AKONADI_FOUND 1)
endif()

if( ${AKONADI_FOUND} )
    set(jamikdeintegration_LIB_SRCS
        ${jamikdeintegration_LIB_SRCS}
        akonadibackend.cpp
    )
endif()

kconfig_add_kcfg_files (jamikdeintegration_LIB_SRCS kcfg_settings.kcfgc)

set(AUTOMOC_MOC_OPTIONS -Muri=org.kde.ringkde.jamikdeintegration)

add_library(jamikdeintegration ${jamikdeintegration_LIB_SRCS} )

target_link_libraries( jamikdeintegration
    # Project
    ringqt
    ringqtquick

    # Qt
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Widgets
    Qt5::QuickControls2

    # KF5
    KF5::Notifications # the Systray
    KF5::I18n
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::ConfigWidgets # KColorScheme
    KF5::XmlGui # KSortcutdialog # (DESKTOP)
    KF5::NotifyConfig # the notification dialog # (DESKTOP)
    KF5::GlobalAccel # (DESKTOP)
)

if ( ${AKONADI_FOUND} )
    target_link_libraries( jamikdeintegration
        KF5::AkonadiCore
        KF5::AkonadiContact
        KF5::Contacts
    )
endif()

set(jamikdeintegration_EXPORT_HDRS
    src/actioncollection.h
    src/windowevent.h
)

# Configure the target config
set(jamikdeintegration_CONFIG_PATH "${CMAKE_CURRENT_BINARY_DIR}/JamiKDEIntegrationConfig.cmake")

if(USES_ADD_SUBDIRECTORY)
   set(JamiKDEIntegration_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
endif()

configure_package_config_file(
   "${CMAKE_CURRENT_SOURCE_DIR}/cmake/JamiKDEIntegrationConfig.cmake.in" ${jamikdeintegration_CONFIG_PATH}
   INSTALL_DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiKDEIntegration/
   PATH_VARS INCLUDE_INSTALL_DIR
)

install( FILES ${jamikdeintegration_CONFIG_PATH}
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiKDEIntegration/
    COMPONENT Devel
)

# Create the target
target_include_directories(jamikdeintegration
    PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include/JamiKDEIntegration>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
)

set_target_properties(jamikdeintegration PROPERTIES
    PUBLIC_HEADER
    "${jamikdeintegration_EXPORT_HDRS}"
)

export(TARGETS jamikdeintegration
    FILE "${PROJECT_BINARY_DIR}/cmake/JamiKDEIntegrationTargets.cmake"
)

install(TARGETS jamikdeintegration
    EXPORT JamiKDEIntegrationTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}" COMPONENT jamikdeintegration
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    COMPONENT Devel
)

install(EXPORT JamiKDEIntegrationTargets
    DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/cmake/JamiKDEIntegration
    COMPONENT Devel
)

install(FILES ring-kde.kcfg DESTINATION ${KCFG_INSTALL_DIR})