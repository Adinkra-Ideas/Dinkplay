cmake_minimum_required(VERSION 3.19)

project(Dinkplay VERSION 0.1 LANGUAGES CXX)

# Required for erasing "missing dsym" error when building archive for app
# store connect. Comment out during development.
# set(QT_USE_RISKY_DSYM_ARCHIVING_WORKAROUND ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Core
    QuickControls2
    Qml
    Widgets
)

qt_standard_project_setup(REQUIRES 6.5)

# define the .qrc files that qt will compile
# to source and store their file paths into
# the MYSOURCES variable for adding as
# in qt_add_executable
qt_add_resources(MYSOURCES
    qml.qrc
)

qt_add_executable(appDinkplay
    main.cpp
    ${MYSOURCES}
)

qt_add_qml_module(appDinkplay
    URI Dinkplay
    VERSION 1.0

    SOURCES
        _main.cpp
        Media.hpp Media.cpp
        Top.hpp Top.cpp
        Directory.hpp Directory.cpp
        ChangePlay.hpp ChangePlay.cpp
        Interval.hpp Interval.cpp
        Player.hpp Player.cpp
        IntervalWorker.hpp IntervalWorker.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDinkplay PROPERTIES
##    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDinkplay
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


#########################################
#       ADD LINKING FLAGS               #
#########################################

# macos and my windows QtCreator finds ./ma_lib/ directory differently
if (IOS)
    target_link_options(appDinkplay BEFORE PUBLIC
        -L./ma_lib      # custom directories to also search fir libraries added using target_link_libraries()
    )
else()
    target_link_options(appDinkplay BEFORE PUBLIC
        -L../../ma_lib  #custom directories to also search fir libraries added using target_link_libraries()
    )
endif()


#########################################
#       ADD LINKED LIBRARIES            #
#########################################

target_link_libraries(appDinkplay PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::QuickControls2
    Qt6::Qml
    Qt6::Widgets
)
if (WIN32)
    target_link_libraries(appDinkplay PRIVATE
        miniaudio_win32
    )
elseif (ANDROID)
    # This filepath is based on default Qt AppData path for windows. Change if on different OS
    include(C:/Users/user/AppData/Local/Android/Sdk/android_openssl/CMakeLists.txt)
    target_sources(appDinkplay PRIVATE
        android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
        android/src/com/denkplay/states/DenkService.java
        android/src/com/denkplay/states/DenkActivity.java
    )

    # set_target_properties() is used to initialize qt macros
    # for buld time.
    set_target_properties(appDinkplay PROPERTIES
       QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/android
    )

    target_link_libraries(appDinkplay PRIVATE
        miniaudio_armv7a
        # miniaudio_arm64
        # miniaudio_x86
        # miniaudio_x86_64
        Qt6::CorePrivate
    )
elseif (IOS)
    set(asset_catalog_path "${CMAKE_SOURCE_DIR}/iOS/Assets.xcassets") #naming the folder ".whatever" makes it pickable as a single file into bundle I believe
    target_sources(appDinkplay PRIVATE
        ${asset_catalog_path}
        ma_lib/miniaudio.h     ma_lib/miniaudio.m
        iOS/src/MyMPRemoteCC.h iOS/src/MyMPRemoteCC.mm
    )
    set_source_files_properties(
        ${asset_catalog_path} PROPERTIES MACOSX_PACKAGE_LOCATION Resources
    )

    # set_target_properties() is used to initialize qt macros
    # for buld time.
    set_target_properties(appDinkplay PROPERTIES
        # Replace Qt's default white splashscreen
        QT_IOS_LAUNCH_SCREEN "${CMAKE_SOURCE_DIR}/iOS/LaunchScr.storyboard"

        MACOSX_BUNDLE TRUE #####

        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/iOS/Info.plist

        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon

        # ADD CUSTOM ENTITLEMENTS FOR ADDING ENTITLEMENTS THAT ARE POSSIBLE TO BE ADDED IN CUSTOM .entitlements file
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_SOURCE_DIR}/iOS/appDinkplay.entitlements

        # ADD XCODE SIGNING FROM CMAKE (THESE CAN BE MANUALLY ENTERED IN XCODE DIRECTLY)
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM 9W37AU7273 #big improvement to prevent the manual sign sign sign
    )


    target_link_libraries(appDinkplay PRIVATE
        #miniaudio_iOSLgplv3 #removed for simulator
        "-framework MediaPlayer"
        "-framework AVFoundation"
    )
endif()


include(GNUInstallDirs)
install(TARGETS appDinkplay
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
