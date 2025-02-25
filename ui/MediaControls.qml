import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    Item {
        id: timeSeeker
        height: 30
        anchors {
            left: parent.left
            right: parent.right
            bottom: mediaControllers.top
        }

        Text {
            id: currentTimeOfAudio
            anchors {
                top: parent.top
                left: parent.left
                leftMargin: 13
            }
            text: Media.focusedAudioCurrTimeString  // will inherit dynamic value from cpp
            color: "white"
            font.pointSize: 10
        }

        Text {
            id: lengthTimeOfAudio
            anchors {
                verticalCenter: currentTimeOfAudio.verticalCenter
                right: parent.right
                rightMargin: 13
            }
            text: Media.focusedAudioLength //"01:50:12"  // will inherit dynamic value from cpp
            color: "white"
            font.pointSize: 10
        }

        Slider {
            id: seekerSlider
            height: 12
            width: parent.width
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
                bottomMargin: 5
            }
            from: 0
            value: 0
            to: 99 // will inherit dynamic value from cpp
            stepSize: 1

            handle: Rectangle {
                x: seekerSlider.leftPadding + seekerSlider.visualPosition * (seekerSlider.availableWidth - width)
                y: seekerSlider.topPadding + seekerSlider.availableHeight / 2 - height / 2
                implicitWidth: (Qt.platform.os == "ios") ? 12 : 12
                implicitHeight: implicitWidth
                radius: implicitWidth * 0.5
            }

            background: Rectangle {
                x: seekerSlider.leftPadding
                y: seekerSlider.topPadding + seekerSlider.availableHeight / 2 - height / 2
                implicitWidth: seekerSlider.availableWidth
                implicitHeight: 2
                width: seekerSlider.availableWidth
                height: implicitHeight
                radius: 2
                color: "#4f4f4f"

                Rectangle {
                    width: seekerSlider.visualPosition * parent.width
                    height: parent.height
                    color: "#1777B7"
                    radius: 2
                }
            }
        }
    }


    GridLayout {
        id: mediaControllers
        height: 40
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        columnSpacing: 10
        flow:  GridLayout.LeftToRight
        columns: 5
        Item {
            id: randomizerComponent
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: ""
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: "qrc:/ui/images/musicIco/prev.png"
                width: 30
                height: 30
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    id: prevPlay
                    anchors.fill: parent
                    onPressed: {
                        Media.change = false
                    }
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: (Media.player === 1) ? "qrc:/ui/images/musicIco/pause.png" : "qrc:/ui/images/musicIco/play.png"
                width: 40
                height: 40
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    id: playPause
                    anchors.fill: parent
                    onPressed: {
                        // If current State is stopped or paused, play. Else pause
                        Media.player = (Media.player === 0 || Media.player === 2) ? 1 : 2
                    }
                }
            }
        }
        Item {
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: "qrc:/ui/images/musicIco/next.png"
                width: 30
                height: 30
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    id: nextPlay
                    anchors.fill: parent
                    onPressed: {
                        Media.change = true
                    }
                }
            }
        }
        Item {
            id: repeatModeComponent
            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Image {
                source: Media.repeat == 0 ? "qrc:/ui/images/musicIco/loop.png" : Media.repeat == 1 ? "qrc:/ui/images/musicIco/loop1.png" : Media.repeat == 2 ? "qrc:/ui/images/musicIco/loop2.png" : ""
                width: 30
                height: 30
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    id: changeRepeatMode
                    anchors.fill: parent
                    onPressed: {
                        if (Media.repeat < 2) {
                            Media.repeat += 1
                        } else {
                            Media.repeat = 0
                        }
                    }
                }
            }
        }
    }
}
