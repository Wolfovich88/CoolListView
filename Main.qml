import QtQuick
import QtQuick.Window
import QtQuick.Controls 2.15

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

                anchors.centerIn: parent
                width: parent.width - font.pixelSize
                font.pixelSize: 20
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    ListView {
        id: view

        readonly property int treshold: 50

        property int prevContentY: -1
        property bool fetched: false

        anchors.fill: parent
        model: coolListModel
        highlightFollowsCurrentItem: false
        highlight: highlighRect
        focus: true

        onContentYChanged: {
            var scrollUp = prevContentY > contentY
            var contentIndex = indexAt(contentX, contentY)
            //Sometimes it misses to fetch items when strict condition contentIndex===treshold is used
            //so, keep this workaround before this issue will be solved
            if (scrollUp && !fetched && (contentIndex <= treshold && contentIndex >= treshold - 5)) {
                fetched = coolListModel.fetchMoreFront()
            }
            else if (fetched) {
                fetched = false
            }

            prevContentY = contentY
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

            onGoToItem:  function(itemId) {
                if (!openPositionById(itemId)) {
                    coolListModel.loadChunkWithItem(itemId)
                    openPositionById(itemId)
                }
            }

            function openPositionById(id) {
                var itemIdIndex = coolListModel.itemIndexById(id);
                if (itemIdIndex >= 0) {
                    view.positionViewAtIndex(itemIdIndex, ListView.Beginning)
                    return true
                }
                return false
            }
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

    Loader {
        id: waitScreenLoader

        anchors.fill: parent
        active: coolListModel && coolListModel.count === 0 && errorScreenLoader.errorString === ""
        sourceComponent: Rectangle {
            color: "yellow"

            Text {
                id: waitText

                anchors.centerIn: parent
                text: "Generating data...."
                font.pixelSize: 20
            }

            ProgressBar {
                id: progressBar

                from: 0.0
                to: 100.0
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: waitText.bottom
                anchors.margins: 10

                Connections {
                    target: coolListModel
                    function onGenerationProgress(progress) {
                        progressBar.value = progress
                    }
                }
            }
        }
    }

    Connections {
        target: coolListModel

        function onError(errorString) {
            if (errorString !== "") {
                errorScreenLoader.errorString = errorString
                editorLoader.active = false
                waitScreenLoader.active = false
                errorScreenLoader.active = true
            }
            else {
                errorScreenLoader.active = false
            }
        }

        function onGenerationStarted() {
            waitScreenLoader.active = true
        }

        function onGenerationFinished() {
            waitScreenLoader.active = false
        }
    }
}
