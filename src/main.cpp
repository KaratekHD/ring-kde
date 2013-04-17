/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

//System
#include <unistd.h>
#include <signal.h>

//Qt
#include <QtCore/QString>

//KDE
#include <KDebug>
#include <kaboutdata.h>
#include <klocale.h>
#include <kde_file.h>

//SFLPhone
#include "sflphoneapplication.h"
#include "klib/configurationskeleton.h"
#include "callview.h"
#include "sflphonecmd.h"

static const char description[] = "A KDE 4 Client for SFLphone";

static const char version[] = "1.2.2";

SFLPhoneApplication* app;
void quitOnSignal(int signal)
{
   Q_UNUSED(signal);
   app->quit();
}

int main(int argc, char **argv)
{
   try
   {
      KLocale::setMainCatalog("sflphone-client-kde");

      KAboutData about(
         "sflphone-client-kde"                      ,
         "sflphone-client-kde"                      ,
         ki18n("SFLphone KDE Client")               ,
         version                                    ,
         ki18n(description)                         ,
         KAboutData::License_GPL_V3                 ,
         ki18n("(C) 2009-2013 Savoir-faire Linux")  ,
         KLocalizedString()                         ,
         "http://www.sflphone.org."                 ,
         "sflphone@lists.savoirfairelinux.net"
      );
      about.addAuthor( ki18n( "Emmanuel Lepage Vallée" ), KLocalizedString(), "emmanuel.lepage@savoirfairelinux.com" );
      about.addAuthor( ki18n( "Jérémy Quentin"         ), KLocalizedString(), "jeremy.quentin@savoirfairelinux.com"  );

      SFLPhoneCmd::parseCmd(argc,argv,about);

      app = new SFLPhoneApplication();

      //dbus configuration
      CallModel::instance();

      KDE_signal(SIGINT  , quitOnSignal);
      KDE_signal(SIGTERM , quitOnSignal);

      int retVal = app->exec();

      ConfigurationSkeleton::self()->writeConfig();

      delete app;
      return retVal;
   }
   catch(const char * msg)
   {
      kDebug() << msg;
   }
   catch(QString& msg)
   {
      kDebug() << msg;
   }
}
