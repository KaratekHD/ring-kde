/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "bookmarkmodel.h"

//Qt
#include <QtCore/QMimeData>

//SFLPhone
#include "kcfg_settings.h"
#include "../lib/historymodel.h"

BookmarkModel* BookmarkModel::m_pSelf = nullptr;

BookmarkModel::BookmarkModel(QObject* parent) : AbstractBookmarkModel(parent)
{
}

BookmarkModel* BookmarkModel::instance()
{
   if (!m_pSelf)
      m_pSelf = new BookmarkModel(nullptr);
   return m_pSelf;
}


void BookmarkModel::addBookmark(const QString& uri, bool trackPresence)
{
   Q_UNUSED(trackPresence)
   ConfigurationSkeleton::setBookmarkList(ConfigurationSkeleton::bookmarkList() << uri);
   reloadCategories();
}

void BookmarkModel::removeBookmark(const QString& uri)
{
   foreach(AbstractBookmarkModel::Subscription* s, m_lTracker) {
      if (s->uri == uri) {
         m_lTracker.removeAll(s);
         break;
      }
   }
   QStringList bookmarks = ConfigurationSkeleton::bookmarkList();
   bookmarks.removeAll(uri);
   ConfigurationSkeleton::setBookmarkList(bookmarks);
}

bool BookmarkModel::displayFrequentlyUsed() const
{
   return ConfigurationSkeleton::displayContactCallHistory();
}

QStringList BookmarkModel::bookmarkList() const
{
   return ConfigurationSkeleton::bookmarkList();
}