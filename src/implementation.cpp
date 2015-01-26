/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include "implementation.h"

//Qt
#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QApplication>

//KDE
#include <KIcon>
#include <KColorScheme>
#include <KLocale>
#include <KStandardDirs>

//Ring
#include <contact.h>
#include <phonenumber.h>
#include <presencestatusmodel.h>
#include <securityvalidationmodel.h>
#include "klib/kcfg_settings.h"
#include <abstractitembackend.h>
#include "icons/icons.h"

QDebug operator<<(QDebug dbg, const Call::State& c)
{
   dbg.nospace() << QString(Call::toHumanStateName(c));
   return dbg.space();
}

QDebug operator<<(QDebug dbg, const Call::DaemonState& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

QDebug operator<<(QDebug dbg, const Call::Action& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

ColorVisitor::ColorVisitor(QPalette pal) : m_Pal(pal) {
   m_Green = QColor(m_Pal.color(QPalette::Base));
   if (m_Green.green()+20 >= 255) {
      m_Green.setRed ( ((int)m_Green.red()  -20));
      m_Green.setBlue( ((int)m_Green.blue() -20));
   }
   else
      m_Green.setGreen(((int)m_Green.green()+20));

   m_Red = QColor(m_Pal.color(QPalette::Base));
   if (m_Red.red()+20 >= 255) {
      m_Red.setGreen(  ((int)m_Red.green()  -20));
      m_Red.setBlue(   ((int)m_Red.blue()   -20));
   }
   else
      m_Red.setRed(    ((int)m_Red.red()     +20));

   m_Yellow = QColor(m_Pal.color(QPalette::Base));
   if (m_Yellow.red()+20 >= 255 || m_Green.green()+20 >= 255) {
      m_Yellow.setBlue(((int)m_Yellow.blue() -20));
   }
   else {
      m_Yellow.setGreen(((int)m_Yellow.green()+20));
      m_Yellow.setRed( ((int)m_Yellow.red()   +20));
   }
}

QVariant ColorVisitor::getColor(const Account* a) {
   if(a->registrationStatus() == Account::State::UNREGISTERED || !a->isEnabled())
      return m_Pal.color(QPalette::Base);
   if(a->registrationStatus() == Account::State::REGISTERED || a->registrationStatus() == Account::State::READY) {
      return m_Green;
   }
   if(a->registrationStatus() == Account::State::TRYING)
      return m_Yellow;
   return m_Red;
}

QVariant ColorVisitor::getIcon(const Account* a) {
   if (a->state() == Account::EditState::MODIFIED)
      return KIcon("document-save");
   else if (a->state() == Account::EditState::OUTDATED) {
      return KIcon("view-refresh");
   }
   return QVariant();
}

void KDEPresenceSerializationVisitor::serialize() {
   PresenceStatusModel* m = PresenceStatusModel::instance();
   QStringList list;
   for (int i=0;i<m->rowCount();i++) {
      QString line = QString("%1///%2///%3///%4///%5")
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Name),Qt::DisplayRole).toString())
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Message),Qt::DisplayRole).toString())
         .arg(qvariant_cast<QColor>(m->data(m->index(i,(int)PresenceStatusModel::Columns::Message),Qt::BackgroundColorRole)).name())    //Color
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Status),Qt::CheckStateRole) == Qt::Checked)    //Status
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Default),Qt::CheckStateRole) == Qt::Checked);   //Default
         list << line;
   }
   ConfigurationSkeleton::setPresenceStatusList(list);
}

void KDEPresenceSerializationVisitor::load() {
   QStringList list = ConfigurationSkeleton::presenceStatusList();
   PresenceStatusModel* m = PresenceStatusModel::instance();

   //Load the default one
   if (!list.size()) {
      PresenceStatusModel::StatusData* data = new PresenceStatusModel::StatusData();
      data->name       = i18n("Online")    ;
      data->message    = i18n("I am available");
      data->status     = true      ;
      data->defaultStatus = true;
      m->addStatus(data);
      data = new PresenceStatusModel::StatusData();
      data->name       = i18n("Away")    ;
      data->message    = i18n("I am away");
      data->status     = false      ;
      m->addStatus(data);
      data = new PresenceStatusModel::StatusData();
      data->name       = i18n("Busy")    ;
      data->message    = i18n("I am busy");
      data->status     = false      ;
      m->addStatus(data);
      data = new PresenceStatusModel::StatusData();
      data->name       = i18n("DND")    ;
      data->message    = i18n("Please do not disturb me");
      data->status     = false      ;
      m->addStatus(data);
   }
   else {
      foreach(const QString& line, list) {
         QStringList fields = line.split("///");
         PresenceStatusModel::StatusData* data = new PresenceStatusModel::StatusData();
         data->name          = fields[(int)PresenceStatusModel::Columns::Name   ];
         data->message       = fields[(int)PresenceStatusModel::Columns::Message];
         data->color         = QColor(fields[(int)PresenceStatusModel::Columns::Color]);
         data->status        = fields[(int)PresenceStatusModel::Columns::Status] == "1";
         data->defaultStatus = fields[(int)PresenceStatusModel::Columns::Default] == "1";
         m->addStatus(data);
      }
   }
}

KDEPresenceSerializationVisitor::~KDEPresenceSerializationVisitor()
{
   
}

bool KDEPresenceSerializationVisitor::isTracked(AbstractItemBackendBase* backend)
{
   Q_UNUSED(backend)
   if (!m_isLoaded) {
      foreach(const QString& str,ConfigurationSkeleton::presenceAutoTrackedCollections()) {
         m_hTracked[str] = true;
      }
      m_isLoaded = true;
   }
   return m_hTracked[backend->name()];
}

void KDEPresenceSerializationVisitor::setTracked(AbstractItemBackendBase* backend, bool tracked)
{
   if (!m_isLoaded) {
      foreach(const QString& str,ConfigurationSkeleton::presenceAutoTrackedCollections()) {
         m_hTracked[str] = true;
      }
      m_isLoaded = true;
   }
   m_hTracked[backend->name()] = tracked;
   QStringList ret;
   for (QHash<QString,bool>::iterator i = m_hTracked.begin(); i != m_hTracked.end(); ++i) {
      if (i.value())
         ret << i.key();
   }
   ConfigurationSkeleton::setPresenceAutoTrackedCollections(ret);
}
