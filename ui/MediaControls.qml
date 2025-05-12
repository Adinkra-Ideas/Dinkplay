import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import com.adinkra.dinkplay 1.0

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
            text: "00:00"
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
            text: Media.secondsToDigitalClock(Media.focusedAudioLengthInt)
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
            to: Media.focusedAudioLengthInt
            stepSize: 1

            // If we had assigned Media.focusedAudioCursorTimeInt directly
            // to value, the slider will keep receiving pulses from the
            // Media.focusedAudioCursorTimeInt cpp when the user is dragging
            // it, forcing it to appear to be jumping away from the user's grip.
            // This connections method allows me check whether user is not
            // dragging the slider, AKA (! seekerSlider.pressed) before
            // allowing the slider receive cpp pulses for changing its position.
            // The (! seekerSlider.waitForRecovery)
            // condition ensures that the slider position only changes through
            // cpp pulses either 1) when new pulse time of +1 second to the one currently
            // in seekerSlider.value is received, after a the user did a seeking action.
            // or 2) when user did a seeking action but before waitForRecovery could be set back
            // to false, they changed play to another audio. In this case, the condition of
            // (Media.focusedAudioCursorTimeInt == 1) will ensure the slider keeps working well.
            // This prevents the slider first jumping back to former time after user does
            // a seeking action, before returning to new time after audio engine starts fully
            // working.
            property int waitForRecovery: 0
            Connections {
                target: Media
                function onFocusedAudioCursorTimeIntChanged() {
                    if ( ! seekerSlider.pressed && ! seekerSlider.waitForRecovery ) {
                        seekerSlider.value = Media.focusedAudioCursorTimeInt
                    } else if ( seekerSlider.waitForRecovery
                               && ( (Media.focusedAudioCursorTimeInt == 1 + seekerSlider.value) || Media.focusedAudioCursorTimeInt == 1) ) {
                        seekerSlider.waitForRecovery = false
                        seekerSlider.value = Media.focusedAudioCursorTimeInt
                    }
                }
            }
            // value pulls integer current time of playing audio
            // from cpp each time the cpp focusedAudioCursorTimeInt
            // changes value. Here, we display it as digital clock
            onValueChanged: {
                currentTimeOfAudio.text = Media.secondsToDigitalClock(value)
            }
            // when user drags the slider, display the moving time
            // in real time
            onMoved: {
                currentTimeOfAudio.text = Media.secondsToDigitalClock(value)
            }
            // Logic for catching onRelease part after slider has being held down,
            // so that we can pass the time to cpp for playing from that point
            onPressedChanged: {
                if ( ! pressed ) {
                    seekerSlider.waitForRecovery = true
                    Media.focusedAudioCursorTimeInt = value
                }
            }

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
                source: (Media.player === MediaEnum.PLAYBACK_PLAYING) ? "qrc:/ui/images/musicIco/pause.png" : "qrc:/ui/images/musicIco/play.png"
                width: 40
                height: 40
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    id: playPause
                    anchors.fill: parent
                    onPressed: {
                        // If current State is stopped or paused, play. Else pause
                        Media.player = (Media.player === MediaEnum.PLAYBACK_STOPPED || Media.player === MediaEnum.PLAYBACK_PAUSED) ? MediaEnum.PLAYBACK_PLAYING : MediaEnum.PLAYBACK_PAUSED
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
