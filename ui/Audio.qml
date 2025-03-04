import QtQuick
// import QtQuick.Controls

// This only scrolls because of ListView default state
// is to scroll. Else, implicitHeight == Window.height
// meant the parent scrollview will always detect this
// child's height to be same as Window.height meaning
// it would have never scrolled.
Item {
    implicitHeight: Window.height
    implicitWidth: Window.width

    ListView {
        id: theListView
        anchors{
            fill: parent
            topMargin: 55       // TopControls height == 40
            leftMargin: 10
            rightMargin: 10
            bottomMargin: 70    // MediaControls height == 70
        }
        clip: true

        property int delegateHeight: 40

        model: Media.audiopaths
        delegate: Item {
            id: theDelegate
            width: theListView.width
            height: theListView.delegateHeight
            Item {
                id: audioNameAligner
                width: theDelegate.width - (32 + 5) /* 32 for option image, 5 for left margin */
                height: theDelegate.height
                anchors.left: theDelegate.left
                anchors.leftMargin: 5
                anchors.top: theDelegate.top
                clip: true
                Text {
                    id: audioName
                    text: modelData.slice(modelData.lastIndexOf('/') + 1)
                    color: theListView.currentIndex === index ? "white" : "grey"
                    font.pointSize: 15
                    anchors.verticalCenter: parent.verticalCenter
                    x: 5
                }
            }
            MouseArea {
                anchors.fill : parent
                onClicked: {
                    Media.playSource = modelData
                }
                onPressAndHold: {
                    overlayMenu.overlayMenuAudioName = audioName.text
                    overlayMenu.overlayMenuAudioIndex = index
                    overlayMenu.toggleOverlayMenuVisibility()
                }
            }
            Image {
                id: itemOption
                width: 32
                height: 32
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                source: "qrc:/ui/images/musicIco/menu_white.png"
                MouseArea {
                    anchors.fill : parent
                    propagateComposedEvents: false
                    onClicked: {
                        overlayMenu.overlayMenuAudioName = audioName.text
                        overlayMenu.overlayMenuAudioIndex = index
                        overlayMenu.toggleOverlayMenuVisibility()
                    }
                }
            }

            /* Set this delegate to currentIndex whenever the PlaybackStateChanged path === path in its modeldata */
            Connections {
                target: Media
                function onPlaybackStateChanged(newPath) {
                    if (newPath === modelData) {
                        theListView.currentIndex = index
                    }
                }
            }
            /* Set this delegate to currentIndex whenever the PlaybackStateChanged path === path in its modeldata */

            SequentialAnimation {
                id: audioNameAnim
                loops: Animation.Infinite
                running: (audioName.width > audioNameAligner.width) && theListView.currentIndex === index
                NumberAnimation { target: audioName; property: "x"; to: -audioName.width; duration: (audioName.width / 25) * 1000 }
                NumberAnimation { target: audioName; property: "x"; to: 5; duration: 5000 }
                onStopped: {
                    audioName.x = 5
                }
            }
        }
        /* highlights the delegate which is currently used as the currentIndex of this listModel */
        highlight: Rectangle { color: "#1777B7"; radius: 2;}
        focus: true
        /* highlights the delegate which is currently used as the currentIndex of this listModel */
    }


    // overlay menu block BEGINS
    Rectangle {
        id: overlayMenu

        property int overlayMenuAudioIndex
        property string overlayMenuAudioName

        anchors {
            fill: parent
            bottomMargin: 70    /* MediaControls height == 70*/
        }
        color: "#b5000000"
        visible: false
        enabled: false

        // for showing and hiding this overlayMenu box
        function toggleOverlayMenuVisibility() {
            overlayMenu.visible = !overlayMenu.visible
            overlayMenu.enabled = !overlayMenu.enabled
        }

        MouseArea {
            anchors.fill : parent
            onClicked: {
                overlayMenu.toggleOverlayMenuVisibility()
            }
        }

        // Rectangle holding all the rows in the overlayMenu BEGINS
        Rectangle {
            id: overlayMenuLists
            width: parent.width
            height: 35 /*topMarginsCombined*/ + audioNameInMenu.contentHeight + removeMenu.height + generateReversed.height
            anchors.bottom: parent.bottom
            color: "white"
            radius: 10
            clip: true

            // Audio name menu item row BEGINS
            Text {
                id: audioNameInMenu
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: 10
                    leftMargin: 15
                    rightMargin: 15
                }
                text: overlayMenu.overlayMenuAudioName
                font.pointSize: 16
                font.bold: true
                wrapMode: Text.WrapAnywhere
            } // Audio name menu item row ENDS

            // Remove menu item row BEGINS
            Rectangle {
                id: removeMenu
                width: parent.width
                height: 40
                anchors {
                    top: audioNameInMenu.bottom
                    topMargin: 10
                }
                color: "#E5E4E2"

                property bool isClicked: true
                property int doHide: 0

                onDoHideChanged: {
                    overlayMenu.toggleOverlayMenuVisibility()
                }

                Behavior on isClicked {
                    SequentialAnimation {
                        ColorAnimation { target: removeMenu; property: "color"; to: "#1777B7"; duration: 200 }
                        ColorAnimation { target: removeMenu; property: "color"; to: "#E5E4E2"; duration: 200 }
                        // The below line is intended to change removeMenu.doHide after the clicked removeMenu
                        // has displayed clicked animation color. Now Changing the value of removeMenu.doHide
                        // will trigger onDoHideChanged which will hide the overlayMenu box
                        NumberAnimation { target: removeMenu; property: "doHide"; to: (!removeMenu.doHide) ? 1 : 0 ; duration: 5 }
                    }
                }
                MouseArea {
                    anchors.fill : parent
                    onClicked: {
                        removeMenu.isClicked = !removeMenu.isClicked
                        Media.deleteAudioPath = overlayMenu.overlayMenuAudioIndex
                        // print(overlayMenu.overlayMenuAudioIndex, " and ", overlayMenu.overlayMenuAudioName, " will now be deleted!")
                    }
                }

                Image {
                    id: removeMenuIcon
                    width: 24
                    height: 24
                    anchors {
                        left: parent.left
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    source: "qrc:/ui/images/musicIco/remove_audio.png"
                }
                Text {
                    anchors {
                        left: removeMenuIcon.right
                        leftMargin: 15
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Remove"
                    font.pointSize: 16
                }

            } // Remove menu item row ENDS

            // Generate-Reversed menu item row BEGINS
            Rectangle {
                id: generateReversed
                width: parent.width
                height: 40
                anchors {
                    top: removeMenu.bottom
                    topMargin: 5
                }
                color: "#E5E4E2"

                property bool isClicked: true
                property int doHide: 0

                onDoHideChanged: {
                    overlayMenu.toggleOverlayMenuVisibility()
                }

                Behavior on isClicked {
                    SequentialAnimation {
                        ColorAnimation { target: generateReversed; property: "color"; to: "#1777B7"; duration: 200 }
                        ColorAnimation { target: generateReversed; property: "color"; to: "#E5E4E2"; duration: 200 }
                        // The below line is intended to change generateReversed.doHide after the clicked generateReversed
                        // has displayed clicked animation color. Now Changing the value of generateReversed.doHide
                        // will trigger onDoHideChanged which will hide the overlayMenu box
                        NumberAnimation { target: generateReversed; property: "doHide"; to: (!generateReversed.doHide) ? 1 : 0 ; duration: 5 }
                    }
                }
                MouseArea {
                    anchors.fill : parent
                    onClicked: {
                        generateReversed.isClicked = !generateReversed.isClicked
                        Media.generateReversedAudioAtByteLevel(overlayMenu.overlayMenuAudioIndex)
                    }
                }


                Image {
                    id: gprIcon
                    width: 24
                    height: 24
                    anchors {
                        left: parent.left
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    source: "qrc:/ui/images/musicIco/gen_reverse.png"
                }
                Text {
                    anchors {
                        left: gprIcon.right
                        leftMargin: 15
                        verticalCenter: parent.verticalCenter
                    }
                    text: "Generate Reversed"
                    font.pointSize: 16
                }
            } // Generate-Reversed menu item row ENDS

            // // Generate-Full-Reversed menu item row BEGINS
            // Rectangle {
            //     id: generateFullReversed
            //     width: parent.width
            //     height: 40
            //     anchors {
            //         top: generateReversed.bottom
            //         topMargin: 5
            //     }
            //     color: "#E5E4E2"


            //     Image {
            //         id: gfrIcon
            //         width: 24
            //         height: 24
            //         anchors {
            //             left: parent.left
            //             leftMargin: 10
            //             verticalCenter: parent.verticalCenter
            //         }
            //         source: "qrc:/ui/images/musicIco/remove_audio.png"
            //     }
            //     Text {
            //         anchors {
            //             left: gfrIcon.right
            //             leftMargin: 15
            //             verticalCenter: parent.verticalCenter
            //         }
            //         text: "Generate Full Reversed"
            //         font.pointSize: 16
            //     }
            //     MouseArea {
            //         anchors.fill : parent
            //         onClicked: {
            //             // removeMenu.isClicked = !removeMenu.isClicked
            //             Media.generateReversedAudioAtBitLevel(overlayMenu.overlayMenuAudioIndex)
            //         }
            //     }
            // } // Generate-Full-Reversed menu item row ENDS



        } // Rectangle holding all the rows in the overlayMenu ENDS

    } // overlay menu block ENDS
}
