import QtQuick
import QtQuick.Window

Window {
    id: root

    width: 480
    height: 640
    visible: true
    title: qsTr("Cool ListView")

    Component {
        id: highlighRect

        Rectangle {
            width: view.currentItem ? view.currentItem.width : 0
            height: view.currentItem  ? view.currentItem.height : 0
            color: "lightsteelblue"
            y: view.currentItem ? view.currentItem.y : 0
        }
    }

    Component {
        id: errorScreenComponent

        Rectangle {
            id: errorScreen

            property alias errorText: errorTxt.text

            anchors.fill: parent
            color: "red"
            opacity: 0.5

            Text {
                id: errorTxt

                font.pixelSize: 20
                anchors.centerIn: parent
            }
        }
    }

    ListView {
        id: view

        property real lastY: 0.0

        anchors.fill: parent
        model: coolListModel
        highlightFollowsCurrentItem: false
        highlight: highlighRect
        focus: true

        onContentYChanged: {
            var contentIndex = indexAt(contentX, contentY)
            var delta = lastY > contentY
            //console.debug("ContentY: " + contentY + " index: " +  contentIndex + "delta: " + delta)
            if (contentIndex === 0 && coolListModel.canFetchMoreFront()) {
                coolListModel.fetchMoreFront()
                positionViewAtIndex(coolListModel.chunkSize, ListView.Beginning)
            }
            lastY = contentY
        }

        delegate: Rectangle {
            id: listDelegate

            width: view.width
            implicitHeight: contentRow.height
            color: "transparent"
            border.width: 1
            border.color: "black"

            Row {
                id: contentRow
                spacing: 20

                height: 30

                Text {
                    id: msgIndexTxt
                    text: messageIndex ? messageIndex : ""
                }

                Text {
                    id: nickTxt
                    text: nickName ? nickName : ""
                }

                Text {
                    id: mgsTxt
                    text: messageText ? messageText : ""
                    wrapMode: Text.Wrap
                }
            }

            MouseArea {
                anchors.fill: parent

                onPressed: {
                    view.currentIndex = index
                }

                onPressAndHold: {
                    editorLoader.active = true
                }
            }
        }
    }

    Loader {
        id: editorLoader

        anchors.fill: parent
        active: false
        sourceComponent: Editor {
            itemIndex: view.currentIndex

            onClose: editorLoader.active = false
        }
    }

    Loader {
        id: errorScreenLoader

        property string errorString: ""

        anchors.fill: parent
        active: false
        sourceComponent: errorScreenComponent

        onLoaded: {
            errorScreenLoader.item.errorText = errorString
        }
    }

    Connections {
        target: coolListModel

        function onError(errorString) {
            if (errorString !== "") {
                errorScreenLoader.errorString = errorString
                editorLoader.active = false
                errorScreenLoader.active = true
            }
        }
    }
}
