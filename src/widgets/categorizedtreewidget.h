/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
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
#ifndef CATEGORIZEDTREEWIDGET_H
#define CATEGORIZEDTREEWIDGET_H

#include <QtGui/QTreeWidget>
#include <QtCore/QDebug>

class KConfigGroup;
class KateColorTreeItem;
class QTreeWidgetItem;

///CategorizedTreeWidget: A better looking widget than the plain QListWidget
class CategorizedTreeWidget : public QTreeWidget
{
  Q_OBJECT
  friend class KateColorTreeItem;
  friend class KateColorTreeDelegate;

  public:
    explicit CategorizedTreeWidget(QWidget *parent = 0);

  public:
    template <class T = QTreeWidgetItem> T* addItem(QString category,bool top = false);
    template <class T = QTreeWidgetItem> T* addCategory(QString name);
    void removeItem(QTreeWidgetItem* item);
    
    QVector<QTreeWidgetItem*> realItems() const;

  Q_SIGNALS:
    void changed();

  protected:
    void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const;
  private:
    QVector<QTreeWidgetItem*> m_lItems;
};

///Add new item to the list
template <class T> T* CategorizedTreeWidget::addItem(QString category,bool top)
{
  QTreeWidgetItem* categoryItem = 0;
  for (int i = 0; i < topLevelItemCount(); ++i) {
    if (topLevelItem(i)->text(0) == category) {
      categoryItem = topLevelItem(i);
      break;
    }
  }

  if (!categoryItem) {
    categoryItem =addCategory(category);
  }
  if (isItemHidden(categoryItem) != false) {
    setItemHidden(categoryItem,false);
//BUG need to be uncommented, see bugs #15620
//     if (indexFromItem(categoryItem).row() < topLevelItemCount()-1) {
//
//     }
  }

  T* iwdg =  new T((top)?0:categoryItem);
  resizeColumnToContents(0);
   if (top)
      categoryItem->insertChild(0,iwdg);
  m_lItems << iwdg;
  return iwdg;
}

///Add a category to the list
template <class T> T* CategorizedTreeWidget::addCategory(QString name)
{
   T* categoryItem = new T(this);
   categoryItem->setFlags(Qt::ItemIsEnabled);
   categoryItem->setText(0, name);
   addTopLevelItem(categoryItem);
   expandItem(categoryItem);
   setItemHidden(categoryItem,true);
   return categoryItem;
}

#endif
