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

import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import org.kde.ringkde.jamiphotoselector 1.0 as JamiPhotoSelector
import org.kde.ringkde.jamivideoview 1.0 as JamiVideoView


Dialog {
    property QtObject person: null
    property QtObject contactMethod: null
    property var individual: null
    property bool isSelected: false
    signal newPhoto(var photo)
    signal done()
    modal: true

    property bool isGlobal: applicationWindow != undefined && (
        typeof(applicationWindow) == "function"
    )

    visible: true
    width:  Math.min(isGlobal ? applicationWindow().width  : 500, 500)
    height: Math.min(isGlobal ? applicationWindow().height : 400, 400)
    standardButtons: Dialog.Ok | Dialog.Cancel
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

    parent: isGlobal ? applicationWindow().contentItem : parent
    x: isGlobal ?
        (applicationWindow().contentItem.width / 2 - width/2) :
        (parent.width / 2 - width/2)
    y: isGlobal ?
        (applicationWindow().height / 2 - height/2) :
        (parent.height / 2 - height/2)

    JamiPhotoSelector.PhotoSelector {
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
                z: -4
                visible: !existingPhoto.hasPixmap
            }

            KQuickItemViews.DecorationAdapter {
                id: existingPhoto
                anchors.centerIn: parent
                pixmap: person ? person.photo : undefined
                z: -3
                width: 128
                height: 128
            }

            Rectangle {
                id: overlay
                z: 100

                JamiVideoView.VideoWidget {
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
                        isSelected = true
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
            Layout.fillWidth: true
            Item {
                height: 1
                Layout.fillWidth: true
            }
            Button {
                text: i18n("From file")
                onClicked: {
                    videoWidget.visible = false
                    fromFile.visible = true
                    fromFile.source = selector.selectFile("")
                }
            }
            Button {
                text: i18n("From camera")
                onClicked: {
                    videoWidget.visible = true
                    fromFile.visible = false

                    if (!RingSession.previewManager.previewing) {
                        RingSession.previewManager.startPreview()
                        videoWidget.started = true
                    }
                    else {
                        RingSession.previewManager.stopPreview()
                        videoWidget.started = false
                        videoWidget.started = false
                    }
                }
            }
            Button {
                text: i18n("Clear")
                onClicked: {
                    videoWidget.visible = false
                    fromFile.visible = false
                    fromFile.source = ""
                    selector.clear()
                    existingPhoto.pixmap = undefined
                }
            }
            Item {
                height: 1
                Layout.fillWidth: true
            }
        }

        Item {
            height: 30
        }
    }

    function applyPhoto() {
        if (person)
            selector.setToPerson(person)
        else if (contactMethod)
            selector.setToContactMethod(contactMethod)
        else if (individual)
            selector.setToIndividual(person)

        videoWidget.started = false
        if (RingSession.previewManager.previewing)
            RingSession.previewManager.stopPreview()
    }

    onAccepted: {
        // It's not obvious that you have to select a square, so do it
        // if the user has not done it

        if ((!isSelected) && (videoWidget.started || fromFile.source != "")) {
            overlay.x = selectorContainer.width/2 - selectorContainer.height/2
            overlay.y = 0
            overlay.width = selectorContainer.height
            overlay.height = selectorContainer.height
            overlay.border.color = "transparent"
            overlay.border.width = 0
            overlay.grabToImage(function(result) {
                selector.image = result.image;
                newPhoto(result.image)
                applyPhoto()
            });
        }

        applyPhoto()

        done()
    }

    onRejected: {
        videoWidget.started = false
        if (RingSession.previewManager.previewing)
            RingSession.previewManager.stopPreview()

        done()
    }
}
