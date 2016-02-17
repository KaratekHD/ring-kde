/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#ifndef PERSONSELECTOR_H
#define PERSONSELECTOR_H

#include "ui_personselector.h"

#include <QtWidgets/QDialog>

class QSortFilterProxyModel;

class ContactMethod;

class PersonSelector : public QDialog, public Ui_PersonSelector
{
   Q_OBJECT
public:
   explicit PersonSelector(QWidget* parent = nullptr, const ContactMethod* cm = nullptr);
   virtual ~PersonSelector() = default;

private:
   QSortFilterProxyModel* m_pSortedContacts {nullptr};
   QSortFilterProxyModel* m_pNearMatchContact {nullptr};

};

#endif
