import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
// import QtMultimedia

Rectangle {
    id: topControls

    property bool playingTab
    signal playSignal;
    required property bool audioTab
    signal audioSignal;
    // property bool videoTab
    // signal videoSignal;
    required property bool settingsTab
    signal settingsSignal;

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Item {
            id: playing
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            visible: Media.player !== 0
            // un-commenting this will cause the page
            // to jump into nowPlaying page whenever
            // a new sound starts playing
            // Connections {
            //     target: Media
            //     function onPlaybackStateChanged() {
            //         if (Media.player === 1) {
            //             topControls.playSignal()
            //         }
            //     }
            // }
            Image {
                source: topControls.playingTab ? "images/musicIco/playing_white.png" : "images/musicIco/playing.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    topControls.playSignal()
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: topControls.audioTab ? "images/musicIco/audio_white.png" : "images/musicIco/audio.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    topControls.audioSignal()
                }
            }
        }
        // Item {
        //     Layout.fillWidth: true;
        //     Layout.fillHeight: true;
        //     Image {
        //         source: topControls.videoTab ? "images/musicIco/video_white.png" : "images/musicIco/video.png"
        //         width: 30
        //         height: 30
        //         anchors.centerIn: parent
        //         fillMode: Image.PreserveAspectFit
        //     }
        //     MouseArea {
        //         anchors.fill: parent
        //         onClicked: {
        //             topControls.videoSignal()
        //             // we will change image here to active
        //         }
        //     }
        // }
        Item {
            id: add
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: "images/musicIco/add.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    folderDialog.open()
                    //fileDialog.open()
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            // When running Interval stops, emit what will change the view to settings
            Connections {
                target: Media
                function onIntervalStatusChanged() {
                    if (!Media.intervalStatus) {
                        topControls.settingsSignal()
                    }
                }
            }
            Image {
                source: topControls.settingsTab ? "images/musicIco/settings_white.png" : "images/musicIco/settings.png"
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    topControls.settingsSignal()
                }
            }
        }
    }

    // called when add is clicked
    FolderDialog {
        id: folderDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]

        // selectedFolder is the folder that the user just selected last.
        // currentFolder is the folder that is first opened by FolderDialog.
        onAccepted: {
            folderDialog.currentFolder = folderDialog.selectedFolder
            Media.currDir = folderDialog.selectedFolder
            print("trrrrrr: ", folderDialog.selectedFolder)
        }

        // currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        // selectedFolder: viewer.folder
    }
    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

        // selectedFile is the file that the user just selected last.
        // currentFolder is the folder that is first opened by FileDialog.
        onAccepted: {
            fileDialog.currentFile = fileDialog.selectedFile
            Media.currDir = fileDialog.selectedFile
            print("trrrrrr: ", fileDialog.selectedFile)
        }
    }

}
