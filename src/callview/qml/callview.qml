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

import net.lvindustries.ringqtquick  1.0 as RingQtQuick
import org.kde.ringkde.jamicallview  1.0 as JamiCallView
import org.kde.ringkde.jamivideoview 1.0 as JamiVideoView
import org.kde.ringkde.jamidialview  1.0 as JamiDialView

Item {
    id: videoDock

    signal callWithVideo ()
    signal callWithAudio ()
    signal callWithScreen()

    // C++ bindings
    property alias    rendererName   : videoWidget.rendererName
    property bool     displayPreview : false
    property string   mode           : "PREVIEW"
    property bool     previewRunning : false
    property alias    peerRunning    : videoWidget.started
    property QtObject call           : null
    property QtObject renderer       : call ? call.renderer : null
    property alias actionFilter      : actionToolbar.filter
    property alias individual: placeholderMessage.individual

    property bool previewVisible: mode != "PREVIEW" &&
        call && RingSession.previewManager.previewing

    Connections {
        target: renderer
        onDestroyed: {
            videoWidget.started = false
        }
    }

    // Let the animations finish before
    Timer {
        id: toolbarTimer
        running: false
        interval: 150
        repeat: false

        onTriggered: {
            actionToolbar.display  = false
            videoSource.visible    = false
            controlToolbar.visible = false
        }
    }

    function showToolbars() {
        actionToolbar.display  = true
        videoSource.visible    = true

        // This toolbar is only useful when there is video
        if (videoWidget.started)
            controlToolbar.visible = true

        videoSource.opacity    = 1
        controlToolbar.opacity = 1
        videoPreview.opacity   = 0.8
        videoSource.anchors.rightMargin    = 0
        controlToolbar.anchors.topMargin   = 0
    }

    function hideToolbars() {
        videoSource.opacity    = 0
        controlToolbar.opacity = 0
        videoPreview.opacity   = 1
        videoSource.anchors.rightMargin    = -20
        controlToolbar.anchors.topMargin   = -20
        toolbarTimer.running = true
    }

    // The main video widget
    JamiVideoView.VideoWidget {
        id: videoWidget
        anchors.fill: parent
        z: -100
        started: false
        visible: started && !hasFailed
        call: videoDock.call
    }

    // The preview
    JamiVideoView.VideoWidget {
        id: videoPreview
        z: -95
        started: false
        visible: previewVisible
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: 192
        height: 108
    }

    // This toolbar allows to rotate video, take screenshots, etc
    JamiVideoView.VideoControlToolbar {
        id: controlToolbar
        anchors.top: parent.top
        visible: false
        Behavior on opacity {
            NumberAnimation {duration: 100}
        }
        Behavior on anchors.topMargin {
            NumberAnimation {duration: 150}
        }
    }

    // The has the currently supported call actions such as hang up
    JamiDialView.ActionToolbar {
        id: actionToolbar

        // Display when there is a single or 2 buttons
        // (happens on new and incoming calls only)
        property bool display: false

        anchors.bottom: parent.bottom
        anchors.bottomMargin: visible ? 0 : -20
        visible: opacity > 0
        opacity: alwaysShow || display ? 1 : 0

        Behavior on opacity {
            NumberAnimation {duration: 100}
        }
        Behavior on anchors.bottomMargin {
            NumberAnimation {duration: 150}
        }
    }

    // Make it obvious when the call is being recording (locally, if the other
    // side choose to record, there is no way to really know.
    JamiCallView.RecordingIcon {
        anchors.right: videoDock.right
        anchors.top: controlToolbar.bottom
    }

    // This allows to switch to different camera or enable screen sharing
    JamiVideoView.VideoSource {
        id: videoSource
        z: 101
        visible: false
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        Behavior on opacity {
            NumberAnimation {duration: 100}
        }
        Behavior on anchors.rightMargin {
            NumberAnimation {duration: 150}
        }
    }

    // The background
    JamiCallView.CallBackground {
        id: placeholderMessage
        z: -99
        anchors.fill: parent
        bottomMargin: actionToolbar.visible ? actionToolbar.height : 0
    }

    // Hide both toolbars when the mouse isn't moving
    //TODO keep visible if the mouse if over the toolbars
    MouseArea {
        id: mainMouseArea
        Timer {
            id: activityTimer
            interval: 3000
            running: true
            repeat: false
            onTriggered: {
                hideToolbars()
            }
        }

        function trackActivity() {
            if (call && mode != "PREVIEW")
                showToolbars()

            activityTimer.restart()
        }

        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        onMouseXChanged: trackActivity()
        onMouseYChanged: trackActivity()
    }

    onModeChanged: {
        if (mode == "PREVIEW") {
            hideToolbars()
            videoWidget.rendererName = "preview"
        }
        else if (mode == "CONVERSATION") {
            videoPreview.started = RingSession.previewManager.previewing
            videoWidget.rendererName = "peer"
        }
        placeholderMessage.mode = mode
    }

    onCallChanged: {
        if (call) {
            placeholderMessage.call = call
            videoSource.call        = call
            controlToolbar.call     = call
        }
        videoWidget.hasFailed = false
        mainMouseArea.visible = call != null || mode == "PREVIEW"
    }

    Connections {
        target: RingSession.previewManager
        onPreviewingChanged: {
            if (mode == "PREVIEW")
                videoWidget.started = RingSession.previewManager.previewing

            videoPreview.started = RingSession.previewManager.previewing
        }
    }

    Connections {
        target: call
    }

    Connections {
        target: call
        onVideoStarted: {
            videoWidget.started = true
        }
        onVideoStopped: {
            videoWidget.started = false
        }
        onLiveMediaIssuesChanaged: {
            // This isn't using properties because the renderer live in their
            // own thread and QML doesn't support this yet
            videoWidget.hasFailed = call.hasIssue(RingQtQuick.Call.VIDEO_ACQUISITION_FAILED)
        }
    }

    Connections {
        target: videoWidget
        onStartedChanged: {
            placeholderMessage.visible = (!videoWidget.started) || (videoWidget.hasFailed)
        }
        onHasFailedChanged: {
            placeholderMessage.visible = (!videoWidget.started) || (videoWidget.hasFailed)
        }
    }

    Connections {
        target: call
        onStateChanged: {
            if (call == null || call.lifeCycleState == RingQtQuick.Call.FINISHED) {
                call = null
                hideToolbars()
            }
        }
    }
}
