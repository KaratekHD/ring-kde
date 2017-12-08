/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
import QtQuick 2.7
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0
import QtQml.Models 2.2
import RingQmlWidgets 1.0
import Ring 1.0
import ContactView 1.0

Item {
    width: contactList.width
    height: 56 + rowCount*(displayNameLabel.implicitHeight+4) + 10
    Rectangle {
        anchors.margins: 5
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        border.width: 1
        border.color: inactivePalette.text
        color: "transparent"
        anchors.fill: parent
        radius: 5

        RowLayout {
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.fill: parent
            Item {
                width:  56
                height: 56
                anchors.verticalCenter: parent.verticalCenter
                ContactPhoto {
                    anchors.fill: parent
                    anchors.margins: 5
                    person: object
    //                 pixmap: decoration
                }
            }

            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    id: displayNameLabel
                    text: display
                    color: activePalette.text
                    font.bold: true
                }
                Text {
                    Layout.fillWidth: true
                    visible: rowCount == 0
                    color: inactivePalette.text
                    text: "TODO"
                }
                Item {
                    Layout.fillHeight: true
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                treeView.selectItem(modelIndex)
                console.log(object)
                if (objectType == 0)
                    contactList.contactMethodSelected(
                        object.lastUsedContactMethod
                    )
                if (objectType == 1)
                    contactList.contactMethodSelected(object)
            }
        }
    }
}
