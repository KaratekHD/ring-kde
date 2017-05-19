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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Ring 1.0

import RingQmlWidgets 1.0

Rectangle {
    property QtObject currentContactMethod: null

    onCurrentContactMethodChanged: {
        primaryName.text = currentContactMethod.primaryName

        photo.pixmap = currentContactMethod.person ?
             currentContactMethod.person.photo : undefined

        bookmarkSwitch.source = currentContactMethod.bookmarked ?
            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
    }

    /*Connections {
        target: currentContactMethod
        onBookmarked: {
            bookmarkSwitch.source = currentContactMethod.bookmarked ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }*/

    color: "gray"
    height: 100
    Layout.fillWidth: true

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8

        Rectangle {
            clip: true
            radius: 5
            height: 90
            width: 90
            color: "white"
            PixmapWrapper {
                id: photo
                anchors.fill: parent
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                Text {
                    id: primaryName
                    font.bold: true
                    font.pointSize: 16
                    text: "My name"
                }
                Image {
                    id: bookmarkSwitch
                    anchors.rightMargin: 1
                    anchors.topMargin: 3
                    height: 16
                    width: 16
                    source: currentContactMethod.bookmarked ? "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                    z: 100
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            mouse.accepted = true
                            currentContactMethod.bookmarked = !currentContactMethod.bookmarked
                            bookmarkSwitch.source = currentContactMethod.bookmarked ?
                                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
            }

            RowLayout {
                Button {
                    id: button
                    text: qsTr("Call")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }

                Button {
                    id: button2
                    text: qsTr("Video")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }

                Button {
                    text: qsTr("Screen sharing")
                    onClicked: {
                        if (currentContactMethod == null) return
                        CallModel.dialingCall(currentContactMethod)
                            .performAction(Call.ACCEPT)
                    }
                }
            }
        }
    }
}
