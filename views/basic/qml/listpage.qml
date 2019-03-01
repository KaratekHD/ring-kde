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

import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Layouts 1.2 as Layouts
import org.kde.ringkde.basicview 1.0 as BasicView
import org.kde.ringkde.jamitroubleshooting 1.0 as JamiTroubleShooting
import org.kde.ringkde.jamisearch 1.0 as JamiSearch

Kirigami.Page {
    id: peerListPage
    property bool displayWelcome: false
    globalToolBarStyle: Kirigami.ApplicationHeaderStyle.ToolBar

    spacing: 0
    leftPadding: 0; rightPadding: 0; topPadding: 0;bottomPadding: 0; padding: 0

    signal search()

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    header: Layouts.ColumnLayout {
        visible: globalTroubleshoot.sourceComponent != null
        height: visible && globalTroubleshoot.active ? implicitHeight : 0
        width: peerListPage.width
        spacing: Kirigami.Units.largeSpacing

        /*
         * This module displays the most severe error (if any) with a bunch
         * of options to fix this. This is only for account or global errors.
         *
         * Individual errors have their own popup, so are media related errors.
         */
        JamiTroubleShooting.GlobalTroubleshoot {
            id: globalTroubleshoot
            Layouts.Layout.fillWidth: true
            Layouts.Layout.margins: Kirigami.Units.largeSpacing
        }

        Item {
            visible: globalTroubleshoot.sourceComponent != null
            height: 10
        }
    }

    /*
     * The searchbox.
     *
     * When clicking on it, it grows and hijack the whole (list) page. It is
     * done this way because the animation are pretty. Otherwise this is so, so
     * wrong.
     */
    titleDelegate: Item {
        id: header
        implicitHeight: parent.parent.height - 2*Kirigami.Units.largeSpacing
        implicitWidth: 10

        JamiSearch.SearchBox {
            id: headerSearchbox
            searchView: _searchView
            anchors.centerIn: parent
            anchors.margins: Kirigami.Units.largeSpacing
            width: parent.width - 2 * Kirigami.Units.largeSpacing
            height: headerSearchbox.focus ? parent.height : parent.height * 1.5
            z: 9999

            Connections {
                target: peerListPage
                onSearch: {
                    headerSearchbox.forceFocus()
                }
            }
        }

        /*
         * Add a blurry background when the search overlay is visible.
         */
        JamiSearch.Overlay {
            id: _searchView
            source: peerListPage
            searchBox: headerSearchbox
            width: peerListPage.width
            height: peerListPage.height + header.height
            x: -(peerListPage.width - header.width)
            y: -Kirigami.Units.largeSpacing

            function forceGeometry() {
                if (!active)
                    return

                width = peerListPage.width
                height = peerListPage.height + header.height
                x = -(peerListPage.width - header.width)
            }

            onDisplayWelcomeChanged: {
                peerListPage.displayWelcome = displayWelcome
            }

            onContactMethodSelected: {
                workflow.currentContactMethod = cm
                var idx = RingSession.peersTimelineModel.individualIndex(cm.individual)
                list.currentIndex = idx.row
            }

            //HACK obey god dammit
            onHeightChanged: _searchView.forceGeometry()
            onActiveChanged: _searchView.forceGeometry()

            //HACK fix breakage caused by the other hack
            Connections {
                target: peerListPage
                onWidthChanged:  _searchView.forceGeometry()
                onXChanged:      _searchView.forceGeometry()
                onHeightChanged: _searchView.forceGeometry()
            }

            z: 9998
        }

        // This happens the very first time the app is used
        onXChanged: _searchView.forceGeometry()
    }

    BasicView.TimelineList {
        id: list
        width: parent.width
        height: parent.height
    }

    /*
    * Display the branding when there's no content
    */
    Image {
        property real size: Math.min(400, list.width*0.75)
        anchors.centerIn: parent
        verticalAlignment: Image.AlignVCenter
        opacity: list.empty == true ? 0.3 : 0
        visible: opacity > 0

        fillMode: Image.PreserveAspectFit
        sourceSize.width:size;sourceSize.height:size;width:size;height:size
        source: "image://SymbolicColorizer/qrc:/sharedassets/branding.svg"

        Behavior on opacity {
            NumberAnimation {duration: 300; easing.type: Easing.InQuad}
        }
    }
}
