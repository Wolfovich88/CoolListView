import QtQuick
import QtQuick.Window

Window {
    id: root

    width: 640
    height: 480
    visible: true
    title: qsTr("Cool ListView")

//    Connections {
//        target: coolListModel

//        function onDataChanged() {
//            console.debug("DATA CHANGED")
//            view.forceLayout()
//        }
//    }
    Component {
        id: highlighRect

        Rectangle {
            width: view.currentItem ? view.currentItem.width : 0
            height: view.currentItem  ? view.currentItem.height : 0
            color: "lightsteelblue"
            y: view.currentItem ? view.currentItem.y : 0
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
                positionViewAtIndex(100, ListView.Beginning)
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

                Text {
                    id: msgIndexTxt
                    text: messageIndex ? messageIndex : ""
                }

                Text {
                    id: nickTxt
                    text: nickName ? nickName : ""
                }

                Text {
                    id: textTxt
                    text: messageText ? messageText : ""
                }
            }

            MouseArea {
                anchors.fill: parent

                onPressed: {
                    view.currentIndex = index
                }
            }
        }
    }
}
