/************************************************************************************
 *   Copyright (C) 2018 by BlueSystems GmbH                                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                            *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "plugin.h"

// Qt
#include <QQmlEngine>
#include <QQmlContext>

// QRC
#include <qrc_jamihistoryview.cpp>

void JamiHistoryView::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QByteArray("org.kde.ringkde.jamihistoryview"));

    qmlRegisterType(QStringLiteral("qrc:/jamihistoryview/qml/callhistory.qml")     , uri, 1, 0, "CallHistory");
    qmlRegisterType(QStringLiteral("qrc:/jamihistoryview/qml/historytimeline.qml") , uri, 1, 0, "HistoryTimeline");
    qmlRegisterType(QStringLiteral("qrc:/jamihistoryview/qml/recordinghistory.qml"), uri, 1, 0, "RecordingHistory");
}

void JamiHistoryView::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine)
    Q_UNUSED(uri)
}
