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
import ContactView 1.0

import RingQmlWidgets 1.0
import PhotoSelectorPlugin 1.0
import org.kde.kirigami 2.2 as Kirigami

Rectangle {
    id: contactHeader
    property QtObject currentContactMethod: null
    property var currentIndividual: null
    property bool isMobile: false
    property bool isCompact: false
    color: "gray"
    height: 70
    Layout.fillWidth: true

    property alias backgroundColor: contactHeader.color
    property var textColor: undefined
    property var cachedPhoto: undefined

    signal selectChat()
    signal selectVideo()

    onCurrentIndividualChanged: {
        if (!currentIndividual) {
            primaryName.text = i18n("N/A")
            contactPhoto.rawIndividual = null
            return
        }

        primaryName.text = currentIndividual ? currentIndividual.bestName : i18n("N/A")

        contactPhoto.rawIndividual = currentIndividual
        bookmarkSwitch.source = (currentIndividual && currentIndividual.hasBookmarks) ?
            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
    }

    onCurrentContactMethodChanged: {
        primaryName.text = currentContactMethod.bestName

        contactPhoto.contactMethod = currentContactMethod

        bookmarkSwitch.source = (currentContactMethod && currentContactMethod.bookmarked) ?
            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
    }

    Connections {
        target: currentIndividual
        onBookmarkedChanged: {
            bookmarkSwitch.source = (currentIndividual && currentIndividual.hasBookmarks) ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }

    Connections {
        target: currentIndividual
        onChanged: {
            primaryName.text = currentIndividual.bestName

            contactPhoto.rawIndividual = currentIndividual

            bookmarkSwitch.source = (currentIndividual && currentIndividual.hasBookmarks) ?
                "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
        }
    }

    PersistentCallControls {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // Wrap the photo as AnchorChanges are unreliable for margins
    Item {
        id: photo

        width: contactHeader.height
        height: contactHeader.height

        ContactPhoto {
            id: contactPhoto
            anchors.fill: parent
            anchors.verticalCenter: contactHeader.verticalCenter
            anchors.margins: contactHeader.state == "" ? 5 : 2

            displayEmpty: false

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                z: 100
                hoverEnabled: true
                onClicked: {
                    var component = Qt.createComponent("PhotoEditor.qml")
                    if (component.status == Component.Ready) {
                        var window = component.createObject(contactHeader)
                        window.contactMethod = currentContactMethod
                    }
                    else
                        console.log("ERROR", component.status, component.errorString())
                }
            }

            Rectangle {
                anchors.fill: parent
                border.width: 1
                border.color: activePalette.text
                radius: 5
                color: "transparent"
                visible: mouseArea.containsMouse || (!contactPhoto.hasPhoto)

                Text {
                    text: i18n("Add\nPhoto")
                    font.bold: true
                    color: activePalette.text
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    Text {
        id: primaryName
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.left: photo.right
        anchors.verticalCenter: contactHeader.verticalCenter
        font.bold: true
        text: "My name"
        color: textColor
        verticalAlignment: Text.AlignVCenter
    }

    Image {
        id: bookmarkSwitch
        anchors.left: primaryName.right
        anchors.verticalCenter: contactHeader.verticalCenter
        anchors.rightMargin: 1
        anchors.topMargin: 3
        anchors.leftMargin: 5
        height: 16
        width: 16
        source: (currentContactMethod && currentContactMethod.bookmarked) ?
            "icons/bookmarked.svg" : "icons/not_bookmarked.svg"
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
        id: separator
        anchors.left: bookmarkSwitch.right
        anchors.verticalCenter: contactHeader.verticalCenter
        width: 10
        height: contactHeader.height - 10
        Rectangle {
            height: parent.height -10
            y: 5
            width: 1
            color: inactivePalette.text
            opacity: 0.2
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    MediaButtons {
        id: mediaButtons
        anchors.verticalCenter: contactHeader.verticalCenter
        anchors.left: separator.right
        anchors.leftMargin: 5
        anchors.rightMargin: 5
    }

    Item {
        id: mediaAvailability
        anchors.left: mediaButtons.right
        anchors.right: contactHeader.right
        anchors.top: contactHeader.top
        anchors.bottom: contactHeader.bottom
        anchors.leftMargin: 5

        // Display reasons why the media buttons are not present
        MediaAvailability {
            defaultSize: parent.height < 48 ? parent.height : 48
            currentIndividual: contactHeader.currentIndividual
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    states: [
        State {
            name: ""
            when: (!isCompact) && (!isMobile)

            PropertyChanges {
                target: contactHeader
                height: 70
            }

            PropertyChanges {
                target: primaryName
                font.pointSize: Kirigami.Theme.defaultFont.pointSize
            }

            PropertyChanges {
                target: separator
                visible: true
            }

            PropertyChanges {
                target: photo
                anchors.margins: 10
            }

            AnchorChanges {
                target: photo
                anchors.horizontalCenter: undefined
                anchors.top: undefined
                anchors.verticalCenter: contactHeader.verticalCenter
                anchors.left: contactHeader.left
            }

            AnchorChanges {
                target: primaryName
                anchors.left: photo.right
                anchors.horizontalCenter: undefined
                anchors.top: undefined
                anchors.right: undefined
                anchors.verticalCenter: contactHeader.verticalCenter
            }

            AnchorChanges {
                target: bookmarkSwitch
                anchors.left: primaryName.right
                anchors.top: undefined
                anchors.right: undefined
                anchors.verticalCenter: contactHeader.verticalCenter
            }

            AnchorChanges {
                target: mediaButtons
                anchors.left: separator.left
                anchors.top: undefined
                anchors.right: undefined
                anchors.verticalCenter: contactHeader.verticalCenter
            }

        },
        State {
            name: "compact"
            extend: ""
            when: isCompact && !isMobile

            PropertyChanges {
                target: contactHeader
                height: 40
            }

            PropertyChanges {
                target: photo
                anchors.margins: 2
            }

            PropertyChanges {
                target: mediaButtons
                visible: false
            }

            PropertyChanges {
                target: separator
                visible: false
            }
        },
        State {
            name: "mobile"
            when: isMobile

            PropertyChanges {
                target: contactHeader
                height: photo.height +
                    mediaButtons.implicitHeight +
                    primaryName.implicitHeight +
                    10 // margins and spacing
            }

            AnchorChanges {
                target: photo
                anchors.horizontalCenter: contactHeader.horizontalCenter
                anchors.top: contactHeader.top
                anchors.left: undefined
                anchors.verticalCenter: undefined
            }

            AnchorChanges {
                target: primaryName
                anchors.top: photo.bottom
                anchors.horizontalCenter: contactHeader.horizontalCenter
                anchors.left: undefined
                anchors.right: undefined
                anchors.verticalCenter: undefined
            }

            AnchorChanges {
                target: bookmarkSwitch
                anchors.left: undefined
                anchors.top: contactHeader.top
                anchors.right: contactHeader.right
                anchors.verticalCenter: undefined
            }

            AnchorChanges {
                target: mediaButtons
                anchors.left: undefined
                anchors.top: undefined
                anchors.right: undefined
                anchors.bottom: contactHeader.bottom
                anchors.verticalCenter: undefined
                anchors.horizontalCenter: contactHeader.horizontalCenter
            }

            AnchorChanges {
                target: mediaAvailability
                anchors.left: contactHeader.left
                anchors.top: contactHeader.top
                anchors.right: undefined
                anchors.bottom: undefined
                anchors.verticalCenter: undefined
                anchors.horizontalCenter: undefined
            }

            PropertyChanges {
                target: photo
                height: 50
                width: 50
                anchors.margins: 2
            }

            PropertyChanges {
                target: primaryName
                font.pointSize: Kirigami.Theme.defaultFont.pointSize*1.5
            }

            PropertyChanges {
                target: separator
                visible: false
            }
        }
    ]
}
