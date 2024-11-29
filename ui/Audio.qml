import QtQuick

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
            bottomMargin: 150    // MediaControls height == 55
        }
        clip: true

        property int delegateHeight: 40

        model: Media.audiopaths
        delegate: Item {
            width: theListView.width
            height: theListView.delegateHeight
            Text {
                text: modelData.slice(modelData.lastIndexOf('/') + 1)
                // color: index % 2 ? "grey" : "yellow"
                color: theListView.currentIndex === index ? "white" : "grey"
                font.pointSize: 12
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 5
            }
            MouseArea {
                anchors.fill : parent
                onClicked: {
                    Media.playSource = modelData
                    /* Set this item to currentIndex onClick */
                    theListView.currentIndex = index
                    /* Set this item to currentIndex onClick */
                }
            }
            Connections {
                target: Media
                function onPlaybackStateChanged() {
                    console.log("received signallllllllll" )
                }
            }
        }
        /* highlights the item at the currentIndex */
        highlight: Rectangle { color: "#1777B7"; radius: 2;}
        focus: true
        /* highlights the item at the currentIndex */
    }
}


// on playing, we will receive the sig inside delegate,
// then we will set the delegate to current index, if its path is same as the path received in sig
// this means playing sig must carry link of path being played
