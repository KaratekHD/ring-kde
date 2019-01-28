/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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

//Parent
#include "systray.h"

//Qt
#include <QtWidgets/QMenu>
#include <QtGui/QIcon>
#include <QtCore/QDebug>
#include <QQuickWindow>

//KDE
#include <QtWidgets/QAction>

// Jami KDE Integration
#include "actioncollection.h"
#include "windowevent.h"
#include "qmlaction.h"

///Constructor
SysTray::SysTray(const QIcon &icon, QWidget *parent)
      : KStatusNotifierItem(parent),
         m_pTrayIconMenu(0)
{
   setIconByPixmap(icon);
   m_pTrayIconMenu = new QMenu(/*parentWidget()*/);
   setContextMenu(m_pTrayIconMenu);
   connect(this,&KStatusNotifierItem::activateRequested,this,&SysTray::slotActivated);

   // The app is always active in the sense there is a network connection and
   // registration status
 #ifndef Q_OS_MAC //FIXME segfault as of mid 2017
   setStatus(KStatusNotifierItem::ItemStatus::Active);
 #endif

   addAction( ActionCollection::instance()->acceptAction  ()->action() );
   addAction( ActionCollection::instance()->holdAction    ()->action() );
   addAction( ActionCollection::instance()->transferAction()->action() );
   addAction( ActionCollection::instance()->recordAction  ()->action() );
   addSeparator();

   addAction( ActionCollection::instance()->closeAction()->action() );

   setObjectName( QStringLiteral("m_pTrayIcon") );
}

///Destructor
SysTray::~SysTray()
{
   //KStatusNotifierItem take ownership of the menu
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Add a new action
void SysTray::addAction(QAction *action)
{
   m_pTrayIconMenu->addAction(action);
}

///Add a menu separator
void SysTray::addSeparator()
{
   m_pTrayIconMenu->addSeparator();
}

void SysTray::slotActivated(bool active, const QPoint& pos)
{
   Q_UNUSED(active)
   Q_UNUSED(pos)

   WindowEvent::instance()->raiseWindow();
}

// kate: space-indent on; indent-width 3; replace-tabs on;
