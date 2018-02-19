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
import QtQuick.Controls 2.2

import PhotoSelectorPlugin 1.0

import Ring 1.0

Dialog {
    property QtObject person: null
    property QtObject contactMethod: null
    signal newPhoto(var photo)
    signal done()
    modal: true

    visible: true
    width: 500
    height: 400
    standardButtons: Dialog.Ok | Dialog.Cancel
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    x: (applicationWindow().width / 2 - width/2) /2 //BUG WTF!
    y: applicationWindow().height / 2 - height/2

    PhotoSelector {
        id: selector
    }

    contentItem: ColumnLayout {

        // The photo fills the item, but is a child of the selection rectangle
        Item {
            id: selectorContainer
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Label {
                anchors.centerIn: parent
                text: i18n("Either select a file or take a selfie")
                z: -2
            }

            Rectangle {
                id: overlay
                z: 100

                VideoWidget {
                    id: videoWidget
                    visible: false
                    z: -1
                }

                Image {
                    id: fromFile
                    z: -2
                    visible: true
                    fillMode: Image.PreserveAspectFit
                }

                color: "transparent"
                border.width: 3
                border.color: "red"
            }

            MouseArea {
                anchors.fill: parent
                property bool track: false
                property int rootX: 0
                property int rootY: 0
                onPressed: {
                    track = true
                    rootX = mouse.x
                    rootY = mouse.y
                    overlay.x = mouse.x
                    overlay.y = mouse.y
                    overlay.width = 0
                    overlay.height = 0
                    fromFile.x = -overlay.x
                    fromFile.y = -overlay.y
                    fromFile.width = selectorContainer.width
                    fromFile.height = selectorContainer.height
                    videoWidget.x = -overlay.x
                    videoWidget.y = -overlay.y
                    videoWidget.width = selectorContainer.width
                    videoWidget.height = selectorContainer.height
                }
                onPositionChanged: {

                    if (track) {
                        var w = 0
                        var h = 0
                        if (mouse.x >= rootX) {
                            overlay.x = rootX
                            w = mouse.x - rootX
                        }
                        else {
                            overlay.x = mouse.x
                            w = rootX - mouse.x
                        }

                        if (mouse.y >= rootY) {
                            overlay.y = rootY
                            h = mouse.y - rootY
                        }
                        else {
                            overlay.y = mouse.y
                            h = rootY - mouse.y
                        }

                        var size = w > h ? h : w

                        overlay.width  = size
                        overlay.height = size
                        fromFile.x = -overlay.x
                        fromFile.y = -overlay.y
                        fromFile.width = selectorContainer.width
                        fromFile.height = selectorContainer.height
                        videoWidget.x = -overlay.x
                        videoWidget.y = -overlay.y
                        videoWidget.width = selectorContainer.width
                        videoWidget.height = selectorContainer.height
                        //FIXME bottom-right -> top-left [x,y]
                    }
                }
                onReleased: {
                    track = false
                    overlay.border.color = "transparent"
                    overlay.border.width = 0
                    overlay.grabToImage(function(result) {
                        selector.image = result.image;
                        newPhoto(result.image)
                        overlay.border.color = "red"
                        overlay.border.width = 3
                    });
                }
            }

            Component.onCompleted: {
                fromFile.x = -overlay.x
                fromFile.y = -overlay.y
                fromFile.width = selectorContainer.width
                fromFile.height = selectorContainer.height
                videoWidget.x = -overlay.x
                videoWidget.y = -overlay.y
                videoWidget.width = selectorContainer.width
                videoWidget.height = selectorContainer.height
            }
        }

        RowLayout {
            Button {
                text: "From file"
                Layout.fillWidth: true
                onClicked: {
                    videoWidget.visible = false
                    fromFile.visible = true
                    fromFile.source = selector.selectFile("")
                }
            }
            Button {
                text: "From camera"
                Layout.fillWidth: true
                onClicked: {
                    videoWidget.visible = true
                    fromFile.visible = false

                    if (!PreviewManager.previewing) {
                        PreviewManager.startPreview()
                        videoWidget.started = true
                    }
                    else {
                        PreviewManager.stopPreview()
                        videoWidget.started = false
                        videoWidget.started = false
                    }
                }
            }
        }

        Item {
            height: 30
        }
    }

    onAccepted: {
        if (person)
            selector.setToPerson(person)
        else if (contactMethod)
            selector.setToContactMethod(contactMethod)

        videoWidget.started = false
        if (PreviewManager.previewing)
            PreviewManager.stopPreview()

        done()
    }

    onRejected: {
        videoWidget.started = false
        if (PreviewManager.previewing)
            PreviewManager.stopPreview()

        done()
    }
}
