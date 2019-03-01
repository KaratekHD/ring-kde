/*
 *   Copyright 2018 Fabian Riethmayer
 *   Copyright 2019 Emmanuel Lepage <emmanuel.lepage@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 3, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
import QtQuick 2.2
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.4
import org.kde.kirigami 2.4 as Kirigami
import QtGraphicalEffects 1.0 as Effect
import org.kde.ringkde.jamicontactview 1.0 as JamiContactView

ListView {
    currentIndex: -1
    id: list
    model: RingSession.peersTimelineModel

    /*
     * Instead of setting the currentIndex directly, listen to the workflow
     * proposed current individual and select this.
     *
     * This is done because sometime, such as when a new call arrives, the
     * selection is change automatically. Avoiding performing direct action
     * limits the amount of business logic required to keep the list selection
     * in sync with the application state.
     */
    Connections {
        target: workflow

        onIndividualChanged: {
            list.currentIndex = RingSession.peersTimelineModel.individualIndex(
                workflow.currentIndividual
            ).row
        }
    }

    delegate: Kirigami.SwipeListItem {
        id: listItem
        onClicked: {
            pageManager.hideCall()
            pageManager.showChat()
            workflow.setIndividual(object)
        }

        activeBackgroundColor: Kirigami.Theme.highlightColor

        highlighted: index == currentIndex

        clip: true

        height: 4 * Kirigami.Units.fontMetrics.height

        contentItem: GridLayout {
            height: 4 * Kirigami.Units.fontMetrics.height

            rows: 4
            columns: 2

            JamiContactView.ContactPhoto {
                id: img
                Layout.margins: 3
                height: 3 * Kirigami.Units.fontMetrics.height
                width: 3 * Kirigami.Units.fontMetrics.height
                Layout.alignment: Qt.AlignVCenter

                individual: object
                defaultColor: highlighted ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor

                drawEmptyOutline: false
                Layout.rowSpan: 4
            }

            Item {
                Layout.fillHeight: true
            }

            Kirigami.Heading {
                level: 3
                text: object.bestName
                color: highlighted ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
            }

            Kirigami.Heading {
                level: 4
                text: object.formattedLastUsedTime
                Layout.fillHeight: true
                Layout.fillWidth: true
                opacity: 0.5
                color: highlighted ?
                    Kirigami.Theme.highlightedTextColor : Kirigami.Theme.disabledTextColor
            }

            Item {
                Layout.fillHeight: true
            }
        }

        actions: [
            actionCollection.callAction,
            actionCollection.mailAction,
        ]
    }
}
