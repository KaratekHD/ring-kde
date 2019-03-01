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
#pragma once

#include <QtCore/QObject>

class KActionCollection;

class QActionBinderPrivate;

/**
 * This class creates QtQuick.Shortcuts from QAction shortcuts. This allows
 * to bind the old configurable shortcuts in a QML application.
 */
class QActionBinder : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QObject* actionCollection READ actionCollection WRITE setActionCollection)

    explicit QActionBinder(QObject* parent = nullptr);
    virtual ~QActionBinder();

    QObject* actionCollection() const;
    void setActionCollection(QObject* ac);
private:
    QActionBinderPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QActionBinder)
};

Q_DECLARE_METATYPE(QActionBinder*)
