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
#include "autocompletion.h"

//Qt
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>

//KDE
#include <klocalizedstring.h>

//Ring
#include <numbercompletionmodel.h>
#include <call.h>
#include <callmodel.h>
#include <delegates/autocompletiondelegate.h>
#include "klib/kcfg_settings.h"

static const int TOOLBAR_HEIGHT = 72;
static const int MARGINS        = 15;

class Handle : public QWidget
{
   Q_OBJECT
public:
   Handle(AutoCompletion* parent = nullptr) : QWidget(parent), m_IsPressed(false),m_Dy(0),m_pParent(parent) {
      installEventFilter(this);
      setMinimumSize(0,8);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
   }
   virtual ~Handle() {}

protected:

   virtual void paintEvent(QPaintEvent* event) override {
      Q_UNUSED(event)
      static const QColor dotCol = QApplication::palette().base().color();
      static const QColor hoverBg = QApplication::palette().highlight().color();
      QPainter p(this);
      p.setBrush(m_IsPressed? hoverBg : dotCol);
      p.setPen(Qt::transparent);
      p.setRenderHint(QPainter::Antialiasing,true);
      p.drawEllipse(width()/2-12,2,5,5);
      p.drawEllipse(width()/2-2,2,5,5);
      p.drawEllipse(width()/2+8,2,5,5);
   }

   virtual bool eventFilter(QObject *obj, QEvent *event) override {
      #pragma GCC diagnostic push
      #pragma GCC diagnostic ignored "-Wswitch-enum"
      switch(event->type()) {
         case QEvent::HoverMove:
         case QEvent::MouseMove:
            if (m_IsPressed) {
               int y = static_cast<QMouseEvent*>(event)->y();
               m_pParent->m_Height = m_pParent->m_Height+(m_Dy-y);
               m_pParent->setMinimumSize(0,m_pParent->m_Height);
               m_pParent->resize(m_pParent->width(),m_pParent->m_Height);
               m_pParent->move(m_pParent->x(),m_pParent->y()-(m_Dy-y));
               m_Dy = y;
            }
            break;
         case QEvent::MouseButtonPress:
            m_Dy = static_cast<QMouseEvent*>(event)->y();
            m_IsPressed = true;
            grabMouse();
            break;
         case QEvent::MouseButtonRelease:
            m_IsPressed = false;
            releaseMouse();
            ConfigurationSkeleton::setAutoCompletionHeight(m_pParent->m_Height);
            break;
         default:
            break;
      }
      #pragma GCC diagnostic pop
      return QObject::eventFilter(obj, event);
   }


private:
   bool m_IsPressed;
   int m_Dy;
   AutoCompletion* m_pParent;
};

AutoCompletion::AutoCompletion(QTreeView* parent) : QWidget(parent),m_Height(125)
{
   m_Height = ConfigurationSkeleton::autoCompletionHeight();
   setVisible(true);
   QVBoxLayout* l = new QVBoxLayout(this);

   Handle* h = new Handle(this);
   l->addWidget(h);

   m_pLabel = new QLabel(this);
   m_pLabel->setText(i18n("Use ⬆ up and ⬇ down arrows to select one of these numbers"));
   m_pLabel->setStyleSheet(QString("color:%1;font-weight:bold;").arg(QApplication::palette().base().color().name()));
   m_pLabel->setWordWrap(true);
   m_pView = new QListView(this);
   l->addWidget(m_pLabel);
   l->addWidget(m_pView);

   m_pModel = new NumberCompletionModel();
   m_pModel->setDisplayMostUsedNumbers(true);
   m_pView->setModel(m_pModel);

   connect(m_pModel,SIGNAL(enabled(bool))  ,this, SLOT(slotVisibilityChange(bool))   );
   connect(m_pModel,SIGNAL(layoutChanged()),this, SLOT(slotLayoutChanged()));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotDoubleClicked(QModelIndex)));

   if (parent) {
      connect(parent->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex)));
      parent->installEventFilter(this);
      QResizeEvent r(size(),size());
      eventFilter(nullptr,&r);
   }
   setMinimumSize(0,m_Height);
   m_pDelegate = new AutoCompletionDelegate();
   m_pView->setItemDelegate(m_pDelegate);
   selectionChanged(CallModel::instance()->selectionModel()->currentIndex());
}

AutoCompletion::~AutoCompletion()
{
   m_pView->setItemDelegate(nullptr);
   delete m_pDelegate;
   delete m_pView;
   delete m_pLabel;
}

void AutoCompletion::moveUp()
{
   const QModelIndex idx = m_pView->selectionModel()->currentIndex();
   if (idx.isValid() ) {
      if (idx.row() != 0)
         m_pView->selectionModel()->setCurrentIndex(m_pModel->index(idx.row()-1,0),QItemSelectionModel::ClearAndSelect);
      else
         m_pView->selectionModel()->setCurrentIndex(QModelIndex(),QItemSelectionModel::Clear);
   }
   else
      m_pView->selectionModel()->setCurrentIndex(m_pModel->index(0,0),QItemSelectionModel::ClearAndSelect);
}

void AutoCompletion::moveDown()
{
   const QModelIndex idx = m_pView->selectionModel()->currentIndex();
   if (idx.isValid() ) {
      if (idx.row() != m_pModel->rowCount()-1)
         m_pView->selectionModel()->setCurrentIndex(m_pModel->index(idx.row()+1,0),QItemSelectionModel::ClearAndSelect);
   }
   else
      m_pView->selectionModel()->setCurrentIndex(m_pModel->index(0,0),QItemSelectionModel::ClearAndSelect);
}

void AutoCompletion::setUseUnregisteredAccounts(bool value) {
   m_pModel->setUseUnregisteredAccounts(value);
}

void AutoCompletion::slotLayoutChanged()
{
   if (!m_pModel->rowCount())
      m_pView->selectionModel()->setCurrentIndex(QModelIndex(),QItemSelectionModel::Clear);
}

void AutoCompletion::selectionChanged(const QModelIndex& idx)
{
   if (!idx.isValid()) {
      setCall(nullptr);
      return;
   }

   Call* call = CallModel::instance()->getCall(idx);
   if (call && call->lifeCycleState() == Call::LifeCycleState::CREATION)
      setCall(call);
   else
      setCall(nullptr);
}

void AutoCompletion::setCall(Call* call)
{
   m_pModel->setCall(call);
}

Call* AutoCompletion::call() const
{
   return m_pModel->call();
}

ContactMethod* AutoCompletion::selection() const
{
   if (isVisible()) {
      const QModelIndex idx = m_pView->selectionModel()->currentIndex();
      if (idx.isValid()) {
         return m_pModel->number(idx);
      }
   }
   return nullptr;
}

bool AutoCompletion::brightOrDarkBase()
{
   const QColor color = palette().base().color();
   return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
}

void AutoCompletion::paintEvent(QPaintEvent* event)
{
   QPainter customPainter(this);
   customPainter.setOpacity(0.1);
   customPainter.setPen(Qt::NoPen);
   customPainter.setRenderHint(QPainter::Antialiasing, true);
   customPainter.setBrush(QBrush(brightOrDarkBase()?Qt::black:Qt::white));
   customPainter.drawRoundedRect(0,0,width(),height(),10,10);
   QWidget::paintEvent(event);
}

bool AutoCompletion::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize) {
      const QWidget* p = parentWidget();

      if (p) {
         int vOffset(0),wOffset(0);
         //If the parent has scrollbar, take this into account
         if (p->inherits("QAbstractScrollArea")) {
            const QAbstractScrollArea* scrl = static_cast< const QAbstractScrollArea*>(p);
            if (scrl && scrl->horizontalScrollBar()->isVisible())
               vOffset += scrl->horizontalScrollBar()->height();
            if (scrl && scrl->verticalScrollBar()->isVisible())
               wOffset += scrl->verticalScrollBar()->width();
         }
         resize(p->width()-wOffset-2*MARGINS,height());
         move(MARGINS,p->height()-TOOLBAR_HEIGHT-vOffset - MARGINS - height());
      }
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

void AutoCompletion::slotVisibilityChange(bool visible)
{
   if (!visible && ((!m_pModel->call()) || m_pModel->call()->lifeCycleState() != Call::LifeCycleState::CREATION))
      m_pModel->setCall(nullptr);
   emit requestVisibility(visible,m_pModel->call()!=nullptr);
}

void AutoCompletion::slotDoubleClicked(const QModelIndex& idx)
{
   qDebug() << "double clicked" << idx;
   emit doubleClicked(selection());
}

void AutoCompletion::reset()
{
   m_pView->selectionModel()->clear();
   setCall(nullptr);
}

#include "autocompletion.moc"
