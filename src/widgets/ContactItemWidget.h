/***************************************************************************
 *   Author : Mathieu Leduc-Hamel mathieu.leduc-hamel@savoirfairelinux.com *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CONTACTITEMWIDGET_H
#define CONTACTITEMWIDGET_H

#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVector>

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <KIcon>
#include <kabc/addressee.h>
#include <kabc/picture.h>
#include <kabc/phonenumber.h>

class QTreeWidgetItem;

class ContactItemWidget : public QWidget
{
   Q_OBJECT
 public:
    ContactItemWidget(QWidget* parent =0);
    ~ContactItemWidget();

    KABC::Addressee* contact() const;
    void setContact(KABC::Addressee& contact);
    static const char * callStateIcons[12];

    QPixmap* getIcon();
    QString  getContactName();
    KABC::PhoneNumber::List getCallNumbers();
    QString  getOrganization();
    QString  getEmail();
    QString  getPicture();
    QTreeWidgetItem* getItem();

    void setItem(QTreeWidgetItem* item);

 private:
    KABC::Addressee m_pContactKA;

    QLabel* m_pIconL;
    QLabel* m_pContactNameL;
    QLabel* m_pCallNumberL;
    QLabel* m_pOrganizationL;
    QLabel* m_pEmailL;

    QTreeWidgetItem* m_pItem;

    bool init;

public slots:
   void updated();
 };

#endif // CALLTREE_ITEM_H
