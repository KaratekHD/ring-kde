/****************************************************************************
 *   Copyright (C) 2016 by Emmanuel Lepage Vallee                           *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef POLLINGPROXY_H
#define POLLINGPROXY_H

#include <QtCore/QIdentityProxyModel>

class QTimer;

/**
 * This proxy add a dataChanged signal at a defined interval. This is useful
 * for small and trivial models where adding proper synchronization add more
 * complexity than it is worth.
 *
 * One use case would be to take a static/constant data source with dates or
 * time and update the elapsed time.
 *
 * Using this model is not recommanded.
 */
class PollingProxy : public QIdentityProxyModel
{
   Q_OBJECT
public:
   explicit PollingProxy(QObject * parent = 0);
   PollingProxy(QAbstractItemModel* model);

private:
   QTimer* m_pTimer {nullptr};

private Q_SLOTS:
   void pollSlot();
};
Q_DECLARE_METATYPE(PollingProxy*)

#endif

//kate: space-indent on; indent-width 3; replace-tabs on;
