import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Rectangle {
    Item {
        height: 25
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Slider {
            id: seekerSlider
            height: 20
            anchors {
                left: parent.left
                leftMargin: 5
                right: parent.right
                rightMargin: 5
            }
            from: 0
            value: 1
            to: 99 // length of audio
            stepSize: 1

            handle: Rectangle {
                x: seekerSlider.leftPadding + seekerSlider.visualPosition * (seekerSlider.availableWidth - width)
                y: seekerSlider.topPadding + seekerSlider.availableHeight / 2 - height / 2
                implicitWidth: (Qt.platform.os == "ios") ? 20 : 20
                implicitHeight: implicitWidth
                radius: implicitWidth * 0.5
            }
        }
    }


    GridLayout {
        height: 55
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
