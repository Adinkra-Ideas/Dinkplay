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
            width: parent.width
            height: theListView.delegateHeight
            Text {
                text: modelData.slice(modelData.lastIndexOf('/') + 1)
                color: index % 2 ? "grey" : "yellow"
                font.pointSize: 12
            }
            MouseArea {
                anchors.fill : parent
                onClicked: {
                    Media.playSource = modelData
                }
            }
        }
    }
}
