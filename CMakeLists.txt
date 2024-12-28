cmake_minimum_required(VERSION 3.19)

project(Dinkplay VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(UI ui)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Core
    QuickControls2
    Qml
    SpatialAudio
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appDinkplay
    main.cpp
)

qt_add_qml_module(appDinkplay
    URI Dinkplay
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES ${UI}/TopControls.qml
        QML_FILES ${UI}/Playing.qml
        QML_FILES ${UI}/Audio.qml
        QML_FILES ${UI}/Video.qml
        QML_FILES ${UI}/Settings.qml
        QML_FILES ${UI}/MediaControls.qml
    RESOURCES
        #${UI}/images/db.mp3
        ${UI}/images/musicSpin.gif
        ${UI}/images/musicIco/artist.png
        ${UI}/images/musicIco/playing.png
        ${UI}/images/musicIco/playing_white.png
        ${UI}/images/musicIco/audio.png
        ${UI}/images/musicIco/audio_white.png
        ${UI}/images/musicIco/video.png
        ${UI}/images/musicIco/video_white.png
        ${UI}/images/musicIco/add.png
        ${UI}/images/musicIco/settings.png
        ${UI}/images/musicIco/settings_white.png
        ${UI}/images/musicIco/rand.png
        ${UI}/images/musicIco/rand_white.png
        ${UI}/images/musicIco/next.png
        ${UI}/images/musicIco/pause.png
        ${UI}/images/musicIco/play.png
        ${UI}/images/musicIco/prev.png
        ${UI}/images/musicIco/loop.png
        ${UI}/images/musicIco/loop1.png
        ${UI}/images/musicIco/loop2.png
        ${UI}/images/musicIco/exercise.gif
    SOURCES
        Media.hpp Media.cpp
        Top.hpp Top.cpp
        Directory.hpp Directory.cpp
        ChangePlay.hpp ChangePlay.cpp
        Interval.hpp Interval.cpp
        Player.hpp Player.cpp
        IntervalWorker.hpp IntervalWorker.cpp
        ma_lib/miniaudio.m # added for simulator
)
# qt_target_qml_sources() provides the ability to add more files to a QML module after qt_add_qml_module() has been called. Typically, you pass the set of .qml files and resources to qt_add_qml_module() directly, but in some cases, it may be desirable, or even necessary, to add files after qt_add_qml_module() has been called. For example, you may wish to add files conditionally based on an if statement expression, or from subdirectories that will only be added if certain criteria are met. You might want to add a set of files with different characteristics to the others, such as a different resource prefix, or with linting and bytecode compilation disabled. The qt_target_qml_sources() command enables these scenarios. https://felgo.com/doc/qt/qt-target-qml-sources/
if (ANDROID)
    qt_target_qml_sources(appDinkplay
        RESOURCES android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
        RESOURCES android/src/com/denkplay/states/DenkService.java
        RESOURCES android/src/com/denkplay/states/DenkActivity.java
    )
endif()


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
        -framework AVFoundation
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
    #Qt6::SpatialAudio this needed to be commented out before app could be built for iOS but I dont know how it got here in first place for android build. Was it a remnant I had forgotten to remove during my time of frustration post qmediaplayer?
)
if (WIN32)
    target_link_libraries(appDinkplay PRIVATE
        miniaudio_win32
    )
elseif (ANDROID)
    # This filepath is based on default Qt AppData path for windows. Change if on different OS
    include(C:/Users/user/AppData/Local/Android/Sdk/android_openssl/CMakeLists.txt)
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
    file(GLOB_RECURSE myAudios CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/ui/images/*.mp3)
    target_sources(appDinkplay PRIVATE ${myAudios})
    set_source_files_properties(
        ${myAudios} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/audios
    )

    set_target_properties(appDinkplay PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/iOS/Info.plist

        #XCODE_ATTRIBUTE_LAUNCH_CONSTRAINT_SELF ${CMAKE_SOURCE_DIR}/iOS/Entitlements.plist.iOS
        #XCODE_ATTRIBUTE_LAUNCH_CONSTRAINT_RESPONSIBLE ${CMAKE_SOURCE_DIR}/iOS/Entitlements.plist.iOS
        #XCODE_ATTRIBUTE_LIBRARY_LOAD_CONSTRAINT ${CMAKE_SOURCE_DIR}/iOS/Entitlements.plist.iOS
        #XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS ${CMAKE_SOURCE_DIR}/iOS/Entitlements.plist.iOS

        # ADD CUSTOM ENTITLEMENTS FOR ADDING ENTITLEMENTS THAT ARE POSSIBLE TO BE ADDED IN CUSTOM .entitlements file
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_SOURCE_DIR}/iOS/appDinkplay.entitlements

        # ADD XCODE SIGNING FROM CMAKE (THESE CAN BE MANUALLY ENTERED IN XCODE DIRECTLY)
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM 9252ZRKKG6 #big improvement to prevent the manual sign sign sign

        # APPSTORE DEPLOYMENT SETTINGS TO PREVENT REJECTION
        #XCODE_ATTRIBUTE_ENABLE_APP_SANDBOX "YES"
        #XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME "YES"
        #XCODE_ATTRIBUTE_ENABLE_USER_SELECTED_FILES "readonly"
        #XCODE_ATTRIBUTE_ALLOW_DYLD_ENVIRONMENT_VARIABLES "YES"
    )


    target_link_libraries(appDinkplay PRIVATE
        #miniaudio_iOS # removed for simulator
    )
endif()


include(GNUInstallDirs)
install(TARGETS appDinkplay
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
