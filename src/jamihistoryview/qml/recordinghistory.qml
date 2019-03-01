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

import org.kde.playground.kquickitemviews 1.0 as KQuickItemViews
import net.lvindustries.ringqtquick 1.0 as RingQtQuick
import net.lvindustries.ringqtquick.models 1.0 as RingQtModels
import org.kde.ringkde.jamitimeline 1.0 as JamiTimeline
import org.kde.ringkde.jamitimelinebase 1.0 as JamiTimelineBase
import org.kde.ringkde.jamiaudioplayer 1.0 as JamiAudioPlayer
import org.kde.ringkde.genericutils 1.0 as GenericUtils
import org.kde.kirigami 2.2 as Kirigami

KQuickItemViews.TreeView {
    id: recordingHistory

    property QtObject individual: null

    signal selectIndex(var idx)

    model: RingQtQuick.TimelineFilter {
        id: filterModel
        individual: recordingHistory.individual
        showMessages: false
        showCalls: false
        showEmptyGroups: true
    }

    clip: true

    // Display something when the chat is empty
    Text {
        color: Kirigami.Theme.textColor
        text: i18n("There is nothing yet, enter a message below or place a call using the buttons\nfound in the header")
        anchors.centerIn: parent
        visible: recordingHistory.empty
        horizontalAlignment: Text.AlignHCenter
    }

    Component {
        id: messageDelegate
        Loader {
            id: chatLoader

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    selectIndex(model.mapToSource(rootIndex))
                }
            }

            property bool showDetailed: false

            // Create a delegate for each type
            Component {
                id: sectionDelegate
                Item {
                    height: content.implicitHeight + 10
                    width: parent.width

                    Rectangle {
                        width: 1
                        color: Kirigami.Theme.textColor
                        height: parent.height
                        anchors.rightMargin: 10
                        anchors.right: parent.right
                    }

                    Rectangle {
                        radius: 99
                        color: Kirigami.Theme.backgroundColor
                        border.width: 1
                        border.color: Kirigami.Theme.textColor
                        width: 16
                        height: 16
                        anchors.right: parent.right
                        anchors.rightMargin: 3

                        Rectangle {
                            id: demandsAttention
                            radius: 99
                            color: Kirigami.Theme.textColor
                            anchors.centerIn: parent
                            height: 8
                            width: 8
                        }
                    }

                    Rectangle {
                        border.color: Kirigami.Theme.textColor
                        border.width: 1
                        anchors.fill: parent

                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        anchors.leftMargin: 5
                        anchors.rightMargin: 30

                        color: "transparent"
                        radius: 10

                        ColumnLayout {
                            id: content
                            anchors.fill: parent

                            Row {
                                Layout.preferredHeight: 2*Kirigami.Units.fontMetrics.height
                                Kirigami.Icon {
                                    height: parent.height
                                    width: parent.height
                                    anchors.margins: 6
                                    source: "dialog-messages"
                                    color: Kirigami.Theme.textColor
                                }
                                Text {
                                    height: parent.height
                                    text: formattedDate

                                    leftPadding: 10

                                    verticalAlignment: Text.AlignVCenter
                                    color: Kirigami.Theme.textColor
                                }
                            }

                            Rectangle {
                                color: Kirigami.Theme.textColor
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                            }

                            Text {
                                Layout.fillWidth: true
                                text: incomingEntryCount + i18n(" incoming messages")

                                Layout.preferredHeight: 2*Kirigami.Units.fontMetrics.height
                                leftPadding: 10

                                verticalAlignment: Text.AlignVCenter
                                color: Kirigami.Theme.textColor
                            }

                            Text {
                                Layout.fillWidth: true
                                text: outgoingEntryCount + i18n(" outgoing messages")

                                Layout.preferredHeight: 2*Kirigami.Units.fontMetrics.height
                                leftPadding: 10

                                verticalAlignment: Text.AlignVCenter
                                color: Kirigami.Theme.textColor
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }
                    }
                }
            }

            Component {
                id: callDelegate
                Item {
                    height: content5555.implicitHeight + 10
                    width: parent.width

                    Behavior on height {
                        NumberAnimation {duration: 200;  easing.type: Easing.OutQuad}
                    }

                    Rectangle {
                        width: 1
                        color: Kirigami.Theme.textColor
                        height: parent.height
                        anchors.rightMargin: 10
                        anchors.right: parent.right
                    }

                    Rectangle {
                        radius: 99
                        color: Kirigami.Theme.backgroundColor
                        border.width: 1
                        border.color: Kirigami.Theme.textColor
                        width: 16
                        height: 16
                        y: 10
                        anchors.right: parent.right
                        anchors.rightMargin: 3

                        Rectangle {
                            id: demandsAttention
                            radius: 99
                            color: Kirigami.Theme.textColor
                            anchors.centerIn: parent
                            height: 8
                            width: 8
                        }
                    }

                    Rectangle {
                        border.color: Kirigami.Theme.textColor
                        border.width: 1
                        anchors.fill: parent

                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        anchors.leftMargin: 5
                        anchors.rightMargin: 30

                        color: "transparent"
                        radius: 10

                        ColumnLayout {
                            id: content5555
                            anchors.fill: parent

                            Row {
                                Layout.preferredHeight: 2*Kirigami.Units.fontMetrics.height
                                Kirigami.Icon {
                                    height: parent.height
                                    width: parent.height
                                    anchors.margins: 6
                                    source: "call-start"
                                    color: Kirigami.Theme.textColor
                                }
                                Text {
                                    height: parent.height
                                    text: formattedDate
                                    color: Kirigami.Theme.textColor

                                    leftPadding: 10

                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            Rectangle {
                                color: Kirigami.Theme.textColor
                                height:1
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                            }

                            JamiTimelineBase.MultiCall {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 32
                                modelIndex: rootIndex
                                count: callCount
                                clip: true
                            }
                        }
                    }
                }
            }

            Component {
                id: categoryDelegate
                Item {
                    height: rect.height
                    JamiTimeline.PeersTimelineCategories {
                        id: rect
                        compact: true
                        section: display
                        recentDate: formattedDate
                        align: Qt.RightEdge
                        anchors.right: parent.right
                    }
                }
            }

            Component {
                id: recordingDelegate
                Item {
                    height: content4444.implicitHeight + 10
                    width: recordingHistory.width

                    Rectangle {
                        width: 1
                        color: Kirigami.Theme.textColor
                        height: parent.height
                        anchors.rightMargin: 10
                        anchors.right: parent.right
                    }

                    Rectangle {
                        radius: 99
                        color: Kirigami.Theme.backgroundColor
                        border.width: 1
                        border.color: Kirigami.Theme.textColor
                        width: 16
                        height: 16
                        anchors.right: parent.right
                        anchors.rightMargin: 3

                        Rectangle {
                            id: demandsAttention
                            radius: 99
                            color: Kirigami.Theme.textColor
                            anchors.centerIn: parent
                            height: 8
                            width: 8
                        }
                    }

                    Rectangle {
                        id: box
                        border.color: Kirigami.Theme.textColor
                        border.width: 1
                        anchors.fill: parent

                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        anchors.leftMargin: 5
                        anchors.rightMargin: 30

                        color: "transparent"
                        radius: 10

                        ColumnLayout {
                            id: content4444
                            anchors.fill: parent

                            Row {
                                Layout.minimumHeight: 2*Kirigami.Units.fontMetrics.height
                                Kirigami.Icon {
                                    height: parent.height
                                    width: parent.height
                                    anchors.margins: 6
                                    source: "media-record"
                                    color: Kirigami.Theme.textColor
                                }
                                Text {
                                    height: parent.height
                                    text: formattedDate
                                    color: Kirigami.Theme.textColor

                                    leftPadding: 10

                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            Rectangle {
                                color: Kirigami.Theme.textColor
                                height:1
                                Layout.preferredHeight: 1
                                Layout.fillWidth: true
                            }

                            JamiAudioPlayer.AudioPlayer {
                                id: rect
                                Layout.minimumWidth: box.width - 10
                            }

                            Item {
                                Layout.fillHeight: true
                            }
                        }
                    }
                }
            }

            // Some elements don't have delegates because they are handled
            // by their parent delegates
            function selectDelegate() {
                if (nodeType == RingQtModels.IndividualTimelineModel.TIME_CATEGORY)
                    return categoryDelegate

                if (nodeType == RingQtModels.IndividualTimelineModel.SECTION_DELIMITER) {
                    return sectionDelegate
                }

                if (nodeType == RingQtModels.IndividualTimelineModel.CALL_GROUP) {
                    return callDelegate
                }

                if (nodeType == RingQtModels.IndividualTimelineModel.RECORDINGS)
                    return recordingDelegate
            }

            sourceComponent: selectDelegate()
        }
    }

    delegate: messageDelegate
}
