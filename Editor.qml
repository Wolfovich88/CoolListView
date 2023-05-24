import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root

    property int itemIndex: -1

    signal close()

    signal goToItem(int itemId)

    Rectangle {
        id: backgroundRect

        anchors.fill: parent
        color: "grey"
        opacity: 0.5
    }

    Column {
        spacing: 10

        anchors.centerIn: parent

        Rectangle {
            width: 150
            height: 30
            color: "white"

            TextInput {
                id: itemIndexEdit

                font.pixelSize: 20
                anchors.fill: parent
                validator: IntValidator{ bottom: 1; top: coolListModel ? coolListModel.totalCount() : 1;}
                inputMethodHints: Qt.ImhDigitsOnly

                onTextChanged: {
                    if (text <= 0)
                        text = 1
                    else if (text > coolListModel.totalCount())
                        text = coolListModel.totalCount()
                }
            }
        }

        Button {
            id: goToItemBtn

            width: 150
            height: 100
            text: "Go to item"
            enabled: itemIndexEdit.text !== ""

            onClicked: {
                root.goToItem(itemIndexEdit.text)
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
