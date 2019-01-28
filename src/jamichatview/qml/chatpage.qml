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
import org.kde.kirigami 2.2 as Kirigami
import QtGraphicalEffects 1.0
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import org.kde.ringkde.jamitimelinebase 1.0 as JamiTimelineBase
import org.kde.ringkde.jamichatview 1.0 as JamiChatView
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

Rectangle {
    id: timelinePage
    property bool showScrollbar: true

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    function focusEdit() {
        chatBox.focusEdit()
    }

    function showNewContent() {
        chatView.moveTo(Qt.BottomEdge)
    }

    function setContactMethod() {
        if (currentIndividual && !currentContactMethod) {
            currentContactMethod = currentIndividual.preferredContactMethod(
                RingQtQuick.Media.TEXT
            )

            if (!currentContactMethod)
                console.log("Cannot find a valid ContactMethod for", currentIndividual)
        }

        return currentContactMethod
    }

    onCurrentIndividualChanged: {
        currentContactMethod = null
        setContactMethod()
    }

    color: Kirigami.Theme.backgroundColor

    property var currentContactMethod: null
    property var currentIndividual: null
    property var timelineModel: null

    property bool canSendTexts: currentIndividual ? currentIndividual.canSendTexts : false

    onTimelineModelChanged: {
        if (!fixmeTimer.running)
            chatView.model = timelineModel
    }

    // Add a blurry background
    ShaderEffectSource {
        id: effectSource
        visible: false

        sourceItem: chatView
        anchors.right: timelinePage.right
        anchors.top: timelinePage.top
        width: scrollbar.fullWidth + 15
        height: chatView.height

        sourceRect: Qt.rect(
            burryOverlay.x,
            burryOverlay.y,
            burryOverlay.width,
            burryOverlay.height
        )
    }

    ColumnLayout {
        anchors.fill: parent
        clip: true

        RowLayout {
            id: chatScrollView
            Layout.fillHeight: true
            Layout.fillWidth: true

            property bool lock: false

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true

                JamiChatView.ChatView {
                    id: chatView
                    width: 600
                    height: parent.height
//                     Layout.alignment: Qt.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    model: null//FIXME timelinePage.timelineModel

                    forceTime: scrollbar.overlayVisible

                    // Due to a race condition, wait a bit, it should be fixed elsewhere,
                    //FIXME but it would take much longer.
                    Timer {
                        id: fixmeTimer
                        repeat: false
                        running: true
                        interval: 33
                        onTriggered: {
                            chatView.model = timelinePage.timelineModel
                        }
                    }
                }

                // It needs to be here due to z-index conflicts between
                // chatScrollView and timelinePage
                Item {
                    id: burryOverlay
                    z: 2
                    visible: false
                    opacity: 0
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.rightMargin: - 15
                    width: scrollbar.fullWidth + 15
                    height: chatView.height
                    clip: true

                    Behavior on opacity {
                        NumberAnimation {duration: 300; easing.type: Easing.InQuad}
                    }

                    Repeater {
                        anchors.fill: parent
                        model: 5
                        FastBlur {
                            anchors.fill: parent
                            source: effectSource
                            radius: 30
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        color: Kirigami.Theme.backgroundColor
                        opacity: 0.75
                    }
                }
            }

            JamiTimelineBase.Scrollbar {
                id: scrollbar
                z: 1000
                bottomUp: true
                Layout.fillHeight: true
                Layout.preferredWidth: 10
                display: chatView.moving || timelinePage.showScrollbar
                model: timelinePage.timelineModel
                view: chatView

                onWidthChanged: {
                    burryOverlay.width = scrollbar.fullWidth + 15
                }

                onOverlayVisibleChanged: {
                    burryOverlay.visible = overlayVisible
                    burryOverlay.opacity = overlayVisible ? 1 : 0
                    effectSource.visible = overlayVisible
                }
            }
        }

        JamiChatView.ChatBox {
            id: chatBox
            Layout.fillWidth: true
            height: 90
            visible: canSendTexts
            RingQtQuick.MessageBuilder {id: builder}
            requireContactRequest: currentContactMethod &&
                currentContactMethod.confirmationStatus == RingQtQuick.ContactMethod.UNCONFIRMED &&
                currentContactMethod.confirmationStatus != RingQtQuick.ContactMethod.DISABLED

            textColor: Kirigami.Theme.textColor
            backgroundColor: Kirigami.Theme.backgroundColor
            emojiColor: Kirigami.Theme.highlightColor

            onDisableContactRequests: {
                if (timelinePage.setContactMethod()) {
                    currentContactMethod.confirmationEnabled = false
                }
            }
        }
    }

    Connections {
        target: chatBox
        onSendMessage: {
            timelinePage.setContactMethod()
            if (currentContactMethod) {
                if (currentContactMethod.account && currentContactMethod.confirmationStatus == ContactMethod.UNCONFIRMED)
                    currentContactMethod.sendContactRequest()

                builder.addPayload("text/plain", message)
                builder.sendWidth(currentContactMethod)
            }
        }
    }
}
