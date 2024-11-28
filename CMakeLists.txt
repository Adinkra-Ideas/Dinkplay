cmake_minimum_required(VERSION 3.16)

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
        ${UI}/images/dinkplay.mp3
    SOURCES
        Media.hpp Media.cpp
        Top.hpp Top.cpp
        Directory.hpp Directory.cpp
        ChangePlay.hpp ChangePlay.cpp
        Interval.hpp Interval.cpp
        Player.hpp Player.cpp
    RESOURCES android/AndroidManifest.xml android/build.gradle android/res/values/libs.xml android/res/xml/qtprovider_paths.xml android/gradle/wrapper/gradle-wrapper.jar android/gradle/wrapper/gradle-wrapper.properties android/gradle.properties android/gradlew android/gradlew.bat
    RESOURCES android/src/com/denkplay/states/DenkService.java
    RESOURCES android/src/com/denkplay/states/DenkActivity.java
    SOURCES IntervalWorker.hpp IntervalWorker.cpp

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appDinkplay PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appDinkplay
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_options(appDinkplay BEFORE PUBLIC -L../../ma_lib)

target_link_libraries(appDinkplay PRIVATE
    Qt6::Quick
    Qt6::Core
    Qt6::QuickControls2
    Qt6::Qml
    Qt6::SpatialAudio
)

include(GNUInstallDirs)
install(TARGETS appDinkplay
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (WIN32)
    target_link_libraries(appDinkplay PRIVATE
        miniaudio_win32
    )
endif()

if (ANDROID)
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
endif()
