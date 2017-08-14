/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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
#include "network.h"

#include <networkinterfacemodel.h>
#include <account.h>

Pages::Network::Network(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   connect(this,&PageBase::accountSet,[this]() {
      m_pPortGroup->setVisible(account()->protocol() == Account::Protocol::SIP);
      m_pPublishedGB->setVisible(account()->protocol() == Account::Protocol::SIP);
      m_pNetworkInterfaces->bindToModel(account()->networkInterfaceModel(),account()->networkInterfaceModel()->selectionModel());
   });
}

// kate: space-indent on; indent-width 3; replace-tabs on;
