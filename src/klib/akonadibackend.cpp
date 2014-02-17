/****************************************************************************
 *   Copyright (C) 2011-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

//Parent
#include "akonadibackend.h"

//Qt
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtCore/QPointer>

//KDE
#include <KDebug>
#include <kdialog.h>
#include <akonadi/control.h>
#include <akonadi/collectionfilterproxymodel.h>
#include <akonadi/kmime/messagemodel.h>
#include <akonadi/recursiveitemfetchjob.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <akonadi/collectionfetchjob.h>
#include <akonadi/collectionfetchscope.h>
#include <akonadi/contact/contacteditor.h>
#include <akonadi/contact/contacteditordialog.h>
#include <akonadi/session.h>
#include <akonadi/monitor.h>
#include <kabc/addressee.h>
#include <kabc/addresseelist.h>
#include <kabc/contactgroup.h>

//SFLPhone library
#include "../lib/contact.h"
#include "../lib/accountlistmodel.h"
#include "../lib/account.h"
#include "../lib/call.h"
#include "../lib/callmodel.h"
#include "../lib/phonenumber.h"
#include "../lib/phonedirectorymodel.h"
#include "../lib/numbercategorymodel.h"
#include "../lib/numbercategory.h"
#include "../lib/contactmodel.h"
#include "kcfg_settings.h"

Akonadi::Session* AkonadiBackend::m_pSession = nullptr;

///Constructor
AkonadiBackend::AkonadiBackend(const Akonadi::Collection& parentCol, QObject* parent) : AbstractContactBackend(parent)
{
   if (!m_pSession)
      m_pSession = new Akonadi::Session( "SFLPhone::instance" );
   m_Coll = parentCol;
} //AkonadiBackend

///Destructor
AkonadiBackend::~AkonadiBackend()
{
   delete m_pJob;
   delete m_pMonitor;
}


bool AkonadiBackend::load()
{
   Akonadi::ItemFetchScope scope;
   scope.fetchFullPayload(true);

   // fetching all collections recursively, starting at the root collection
   m_pJob = new Akonadi::ItemFetchJob( m_Coll, this );
   m_pJob->setFetchScope(scope);
//    m_pJob->fetchScope().setContentMimeTypes( QStringList() << "text/x-vcard" );
   connect( m_pJob, SIGNAL(itemsReceived(Akonadi::Item::List)), this, SLOT(itemsReceived(Akonadi::Item::List)) );

   //Configure change monitor
   m_pMonitor = new Akonadi::Monitor(this);
   m_pMonitor->fetchCollectionStatistics(false);
   m_pMonitor->setItemFetchScope(scope);
   connect(m_pMonitor,SIGNAL(itemAdded(Akonadi::Item,Akonadi::Collection)),
      this,SLOT(slotItemAdded(Akonadi::Item,Akonadi::Collection)));
   connect(m_pMonitor,SIGNAL(itemChanged(const Akonadi::Item,const QSet<QByteArray>)),
      this,SLOT(slotItemChanged(const Akonadi::Item,const QSet<QByteArray>)));
   connect(m_pMonitor,SIGNAL(itemRemoved(const Akonadi::Item)),
      this,SLOT(slotItemRemoved(const Akonadi::Item)));


   m_pMonitor->setCollectionMonitored(m_Coll,true);
   return true;
}

bool AkonadiBackend::reload()
{
   //TODO
   return false;
}

AbstractContactBackend::SupportedFeatures AkonadiBackend::supportedFeatures() const
{
   return (AbstractContactBackend::SupportedFeatures) (
      AbstractContactBackend::SupportedFeatures::NONE |
      AbstractContactBackend::SupportedFeatures::LOAD |
      AbstractContactBackend::SupportedFeatures::SAVE |
      AbstractContactBackend::SupportedFeatures::EDIT |
      AbstractContactBackend::SupportedFeatures::ADD  );
}

/*****************************************************************************
 *                                                                           *
 *                                  Helper                                   *
 *                                                                           *
 ****************************************************************************/

///Convert string to akonadi KABC::PhoneNumber
KABC::PhoneNumber::Type AkonadiBackend::nameToType(const QString& name)
{
   if      (name == "Home"   ) return KABC::PhoneNumber::Home ;
   else if (name == "Work"   ) return KABC::PhoneNumber::Work ;
   else if (name == "Msg"    ) return KABC::PhoneNumber::Msg  ;
   else if (name == "Pref"   ) return KABC::PhoneNumber::Pref ;
   else if (name == "Voice"  ) return KABC::PhoneNumber::Voice;
   else if (name == "Fax"    ) return KABC::PhoneNumber::Fax  ;
   else if (name == "Cell"   ) return KABC::PhoneNumber::Cell ;
   else if (name == "Video"  ) return KABC::PhoneNumber::Video;
   else if (name == "Bbs"    ) return KABC::PhoneNumber::Bbs  ;
   else if (name == "Modem"  ) return KABC::PhoneNumber::Modem;
   else if (name == "Car"    ) return KABC::PhoneNumber::Car  ;
   else if (name == "Isdn"   ) return KABC::PhoneNumber::Isdn ;
   else if (name == "Pcs"    ) return KABC::PhoneNumber::Pcs  ;
   else if (name == "Pager"  ) return KABC::PhoneNumber::Pager;
   return KABC::PhoneNumber::Home;
}

void AkonadiBackend::fillContact(Contact* c, const KABC::Addressee& addr) const
{
   c->setNickName       (addr.nickName()       );
   c->setFormattedName  (addr.formattedName()  );
   c->setFirstName      (addr.givenName()      );
   c->setFamilyName     (addr.familyName()     );
   c->setOrganization   (addr.organization()   );
   c->setPreferredEmail (addr.preferredEmail() );
   c->setDepartment     (addr.department()     );
   c->setUid            (addr.uid().toUtf8()   );

   const KABC::PhoneNumber::List numbers = addr.phoneNumbers();
   Contact::PhoneNumbers newNumbers(c);
   foreach (const KABC::PhoneNumber& number, numbers) {
      newNumbers << PhoneDirectoryModel::instance()->getNumber(number.number(),c,nullptr,number.typeLabel());
      QString number2 = number.number();
      if (number2.left (5) == "<sip:")
         number2 = number2.remove(0,5);
      if (number2.right(1) == ">"    )
         number2 = number2.remove(number2.size()-2,1);
   }
   c->setPhoneNumbers   (newNumbers           );
}

Contact* AkonadiBackend::addItem(Akonadi::Item item, bool ignoreEmpty)
{
   Contact* aContact = nullptr;
   if ( item.hasPayload<KABC::Addressee>() ) {
      m_pMonitor->setItemMonitored(item,true);
      KABC::Addressee tmp = item.payload<KABC::Addressee>();
      const KABC::PhoneNumber::List numbers = tmp.phoneNumbers();
      const QString uid = tmp.uid();
      if (numbers.size() || !ignoreEmpty) {
         aContact   = new Contact(this);

         //This need to be done first because of the phone numbers indexes
         fillContact(aContact,tmp);

         if (!tmp.photo().data().isNull())
            aContact->setPhoto(new QPixmap(QPixmap::fromImage( tmp.photo().data()).scaled(QSize(48,48))));
         else
            aContact->setPhoto(0);

         m_AddrHash[ uid ] = tmp ;
         m_ItemHash[ uid ] = item;
      }
   }
   return aContact;
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Update the contact list when a new Akonadi collection is added
void AkonadiBackend::update(const Akonadi::Collection& collection)
{
   if ( !collection.isValid() ) {
      kDebug() << "The current collection is not valid";
      return;
   }

   const bool onlyWithNumber =  ConfigurationSkeleton::hideContactWithoutPhone();

   Akonadi::RecursiveItemFetchJob *job = new Akonadi::RecursiveItemFetchJob( collection, QStringList() << KABC::Addressee::mimeType() << KABC::ContactGroup::mimeType());
   job->fetchScope().fetchFullPayload();
   if ( job->exec() ) {
      const Akonadi::Item::List items = job->items();

      foreach ( const Akonadi::Item &item, items ) {
         Contact* c = addItem(item,onlyWithNumber);
         ContactModel::instance()->addContact(c);
      }
   }
//    return m_ContactByUid.values();
} //update

///Edit backend value using an updated frontend contact
bool AkonadiBackend::edit(Contact* contact,QWidget* parent)
{
   Akonadi::Item item = m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      kDebug() << "Contact not found";
      return false ;
   }

   if ( item.isValid() ) {
      QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::EditMode, parent );
      editor->loadContact(item);
      QPointer<KDialog> dlg = new KDialog(parent);
      dlg->setMainWidget(editor);
      if ( dlg->exec() == QDialog::Accepted ) {
         if ( !editor->saveContact() ) {
            delete dlg;
            kDebug() << "Unable to save new contact to storage";
            return false;
         }
      }
      delete editor;
      delete dlg   ;
      return true;
   }
   return false;
} //editContact

///Save a contact
bool AkonadiBackend::save(const Contact* contact)
{
   Akonadi::Item item = m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      kDebug() << "Contact not found";
      return false;
   }
   KABC::Addressee payload = item.payload<KABC::Addressee>();
   payload.setNickName       ( contact->nickName()        );
   payload.setFormattedName  ( contact->formattedName()   );
   payload.setGivenName      ( contact->firstName()       );
   payload.setFamilyName     ( contact->secondName()      );
   payload.setOrganization   ( contact->organization()    );
   payload.setDepartment     ( contact->department()      );

   foreach (PhoneNumber* nb, contact->phoneNumbers()) {
      KABC::PhoneNumber pn;
      pn.setType(nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      payload.insertPhoneNumber(pn);
   }
   //TODO save the contact
   return false;
}


bool AkonadiBackend::append(const Contact* item)
{
   Q_UNUSED(item)
   return false;
}

///Add a new contact
bool AkonadiBackend::addNewContact(Contact* contact,QWidget* parent)
{
   KABC::Addressee newContact;
   newContact.setNickName       ( contact->nickName()        );
   newContact.setFormattedName  ( contact->formattedName()   );
   newContact.setGivenName      ( contact->firstName()       );
   newContact.setFamilyName     ( contact->secondName()      );
   newContact.setOrganization   ( contact->organization()    );
   newContact.setDepartment     ( contact->department()      );
   //newContact.setPreferredEmail ( contact->getPreferredEmail()  );//TODO

   foreach (PhoneNumber* nb, contact->phoneNumbers()) {
      KABC::PhoneNumber pn;
      pn.setType(nameToType(nb->category()->name()));

      pn.setNumber(nb->uri());
      newContact.insertPhoneNumber(pn);
   }

   //aContact->setPhoneNumbers   (newNumbers           );//TODO

   QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::CreateMode, parent );

   editor->setContactTemplate(newContact);

   QPointer<KDialog> dlg = new KDialog(parent);
   dlg->setMainWidget(editor);
   if ( dlg->exec() == QDialog::Accepted ) {
      if ( !editor->saveContact() ) {
         delete dlg;
         kDebug() << "Unable to save new contact to storage";
         return false;
      }
   }
   delete dlg;
   return true;
} //addNewContact

///Implement virtual pure method
bool AkonadiBackend::edit(Contact* contact)
{
   return edit(contact,nullptr);
}

///Implement virtual pure method
bool AkonadiBackend::addNew(Contact* contact)
{
   return addNewContact(contact,nullptr);
}

///Add a new phone number to an existing contact
bool AkonadiBackend::addPhoneNumber(Contact* contact, PhoneNumber* number)
{
   Akonadi::Item item = m_ItemHash[contact->uid()];
   if (!(item.hasPayload<KABC::Addressee>() && item.payload<KABC::Addressee>().uid() == contact->uid())) {
      kDebug() << "Contact not found";
      return false;
   }
   if ( item.isValid() ) {
      KABC::Addressee payload = item.payload<KABC::Addressee>();
      payload.insertPhoneNumber(KABC::PhoneNumber(number->uri(),nameToType(number->category()->name())));
      item.setPayload<KABC::Addressee>(payload);
      QPointer<Akonadi::ContactEditor> editor = new Akonadi::ContactEditor( Akonadi::ContactEditor::EditMode, (QWidget*)nullptr );
      editor->loadContact(item);

      QPointer<KDialog> dlg = new KDialog(nullptr);
      dlg->setMainWidget(editor);
      if ( dlg->exec() == QDialog::Accepted ) {
         if ( !editor->saveContact() ) {
            delete dlg;
            kDebug() << "Unable to save new contact to storage";
            return false;
         }
      }
      delete dlg   ;
      delete editor;
      return true;
   }
   else {
      kDebug() << "Invalid item";
      return false;
   }
}


/*****************************************************************************
 *                                                                           *
 *                                    Slots                                  *
 *                                                                           *
 ****************************************************************************/

///Called when a new collection is added
void AkonadiBackend::itemsReceived( const Akonadi::Item::List& list)
{
//    QList<int> disabledColl = ConfigurationSkeleton::disabledCollectionList();
   foreach (const Akonadi::Item& item, list) {
//       if (disabledColl.indexOf(coll.id()) == -1) {
//          update(coll);
//          emit reloaded();
//       }
      slotItemAdded(item,m_Coll);
   }
}

///Callback when a new item is added
void AkonadiBackend::slotItemAdded(const Akonadi::Item& item,const Akonadi::Collection& coll)
{
   Q_UNUSED(coll)
   Contact* c = addItem(item,ConfigurationSkeleton::hideContactWithoutPhone());
   if (c) { //Not all items will have an addressee payload
      emit newContactAdded(c);
   }
}

///Callback when an item change
void AkonadiBackend::slotItemChanged(const Akonadi::Item &item, const QSet< QByteArray > &part)
{
   Q_UNUSED(part)
   if (item.hasPayload<KABC::Addressee>()) {
      KABC::Addressee tmp = item.payload<KABC::Addressee>();
      Contact* c = ContactModel::instance()->getContactByUid(tmp.uid().toUtf8());
      if (c)
         fillContact(c,tmp);
   }
}

///Callback when a contact is removed
void AkonadiBackend::slotItemRemoved(const Akonadi::Item &item)
{
   Contact* c = ContactModel::instance()->getContactByUid(item.remoteId().toUtf8());
   ContactModel::instance()->disableContact(c);
}
