import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    property int itemIndex: -1

    signal close()

    Rectangle {
        id: backgroundRect

        anchors.fill: parent
        color: "grey"
        opacity: 0.5
    }

    Column {
        spacing: 10

        anchors.centerIn: parent

        Button {
            id: addBtn
            width: 150
            height: 100
            text: "Add after"

            onClicked: {
                if (itemIndex >= 0) {
                    coolListModel.addItem(itemIndex + 1)
                }
                close()
            }
        }

        Button {
            id: removeBtn
            width: 150
            height: 100
            text: "Delete"

            onClicked: {
                coolListModel.remove(itemIndex)
                close()
            }
        }

        Button {
            id: cancelBtn
            width: 150
            height: 100
            text: "Cancel"

            onClicked: {
                close()
            }
        }
    }
}
