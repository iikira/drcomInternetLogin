cmake_minimum_required(VERSION 3.14)

project(drcomInternetLogin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Widgets REQUIRED)

set(COMMON_SOURCES
        drcom_worker.h drcom_worker.cpp
        drcom_controller.h drcom_controller.cpp
        defer.h defer.cpp
        utils.h utils.cpp
        settings.h settings.cpp
        )

set(PROJECT_SOURCES
        main.cpp
        qml.qrc
        ${COMMON_SOURCES}
        )

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(drcomInternetLogin
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            )
else ()
    if (ANDROID)
        add_library(drcomInternetLogin SHARED
                ${PROJECT_SOURCES}
                )
    else ()
        add_executable(drcomInternetLogin
                ${PROJECT_SOURCES}
                )
    endif ()
endif ()

# android openssl
if (ANDROID)
    if (NOT DEFINED ANDROID_OPENSSL_DIR)
        set(ANDROID_OPENSSL_DIR ${ANDROID_SDK_ROOT}/android_openssl)
    endif ()
    include(${ANDROID_OPENSSL_DIR}/CMakeLists.txt)
endif ()

target_compile_definitions(drcomInternetLogin
        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(drcomInternetLogin
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Widgets
        )
if (ANDROID)
    target_link_libraries(drcomInternetLogin
            PRIVATE
            )
endif ()

set_target_properties(drcomInternetLogin PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )

if (QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(drcomInternetLogin)
    qt_finalize_executable(drcomInternetLogin)
endif ()

if (NOT ANDROID)
    add_executable(drcomInternetLoginCore
            console.cpp
            ${COMMON_SOURCES}
            )
    target_link_libraries(drcomInternetLoginCore
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Network
            )
endif ()
