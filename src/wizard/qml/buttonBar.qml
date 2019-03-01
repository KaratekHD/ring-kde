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
import QtQuick 2.0
import QtQuick.Controls 2.0 as Controls
import QtQuick.Layouts 1.3
import net.lvindustries.ringqtquick 1.0 as RingQtQuick

Item {
    id: buttonBar

    property alias backButton: backButton
    property alias nextButton: nextButton
    property bool  displaySkip: RingSession.accountModel.size > 0
    property bool  displayBusy: false

    signal skip()

    Rectangle {
        id: rectangle
        color: "#0886a0"
        anchors.fill: parent

        // Left buttons
        RowLayout {
            anchors.fill: parent

            // Align left
            Controls.Button {
                id: backButton
                Layout.fillHeight: true
                text: i18n("Back")
                //visible: false //FIXME
            }

            Item {
                Layout.fillWidth: true
            }

            // Align right

            Controls.BusyIndicator {
                id: busyIndicator
                visible: false
                Layout.fillHeight: true
            }

            Controls.Label {
                id: missingFields
                text: i18n("Please fill the required fields")
                verticalAlignment: Text.AlignVCenter
                color: "#ff1111"
                font.bold: true
                visible: true
                Layout.fillHeight: true
            }

            Controls.Button {
                id: nextButton
                Layout.fillHeight: true
                text: i18n("Next")
                visible: false
            }

            Controls.Button {
                id: skipButton
                Layout.fillHeight: true
                text: i18n("Skip")
                visible: displaySkip
                onClicked: {
                    skip()
                }
            }
        }
    }
    states: [
        State {
            name: "shown"

            PropertyChanges {
                target: buttonBar
                visible: true
                state: "nextAvailable"
            }
        },

        State {
            name: "locked"

            PropertyChanges {
                target: buttonBar
                anchors.right: buttonBar.parent.right
                anchors.left: buttonBar.parent.left
                anchors.bottom: buttonBar.parent.bottom
                anchors.leftMargin: 0
                visible: true
            }
        },
        State {
            name: "firstStep"

            PropertyChanges {
                target: backButton
                visible: false
            }
        },
        State {
            name: "busy"
            extend: "locked"

            PropertyChanges {
                target: missingFields
                text: i18n("Please wait")
            }

            PropertyChanges {
                target: busyIndicator
                visible: true
            }
        },
        State {
            name: "nextAvailable"
            extend: "locked"
            PropertyChanges {
                target: nextButton
                visible: true
            }
            PropertyChanges {
                target: missingFields
                visible: false
            }
        },

        State {
            name: i18n("finish")
            extend: "nextAvailable"
            PropertyChanges {
                target: skipButton
                visible: false
            }

            PropertyChanges {
                target: nextButton
                text: "Finish"
            }
        }
    ]

    /*transitions: [
        Transition {
            from: "*"; to: "shown"

            // Slide-in the new account form
            NumberAnimation {
                target: buttonBar
                easing.type: Easing.OutQuad
                properties: "y";
                duration: 700
            }
        }
    ]*/
}
