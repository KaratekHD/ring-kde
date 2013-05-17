/***************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#include "callviewoverlaytoolbar.h"

//Qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QHBoxLayout>
#include <QtGui/QStyle>
#include <QDebug> //TODO remove

//KDE
#include <KStandardDirs>
#include <KIcon>

//SFLPhone
#include "sflphone.h"
#include "extendedaction.h"
#include <klib/tipmanager.h>
#include <lib/call.h>

const TypedStateMachine< TypedStateMachine< bool , Call::State, Call::State::COUNT > , ActionButton, ActionButton::COUNT > visibility = {{              /*ROW = BUTTONS   COLS=STATE*/
            /* INCOMING  RINGING CURRENT DIALING  HOLD FAILURE BUSY  TRANSFERRED TRANSF_HOLD  OVER  ERROR CONFERENCE CONFERENCE_HOLD:*/
 /*PICKUP   */ {{ true   , true ,  false,  false, false, false, false,   false,     false,    false, false,  false,      false    }},
 /*HOLD     */ {{ false  , false,  true ,  false, false, false, false,   true ,     false,    false, false,  true ,      false    }},
 /*UNHOLD   */ {{ false  , false,  false,  false, true , false, false,   false,     false,    false, false,  false,      false    }},
 /*HANGUP   */ {{ false  , true ,  true ,  false, true , true , true ,   true ,     true ,    false, true ,  true ,      true     }},
 /*MUTE     */ {{ false  , true ,  true ,  false, false, false, false,   false,     false,    false, false,  false,      false    }},
 /*TRANSFER */ {{ false  , false,  true ,  false, true , false, false,   false,     false,    false, false,  false,      false    }},
 /*RECORD   */ {{ false  , true ,  true ,  false, true , false, false,   true ,     true ,    false, false,  true ,      true     }},
 /*REFUSE   */ {{ true   , false,  false,  false, false, false, false,   false,     false,    false, false,  false,      false    }},
 /*ACCEPT   */ {{ false  , false,  false,  true , false, false, false,   false,     false,    false, false,  false,      false    }},
}};

///Constructor
CallViewOverlayToolbar::CallViewOverlayToolbar(QTreeView* parent) : QWidget(parent),m_pRightRender(0),m_pLeftRender(0),m_pParent(parent)
{
   m_pRightRender = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_right_corner.svg") );
   m_pLeftRender  = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_left_corner.svg" ) );

   QHBoxLayout* layout = new QHBoxLayout(this);

   m_pHold     = createButton( SFLPhone::app()->getHoldAction()     );
   m_pUnhold   = createButton( SFLPhone::app()->getUnholdAction()   );
   m_pMute     = createButton( SFLPhone::app()->getMuteAction()     );
   m_pPickup   = createButton( SFLPhone::app()->getPickupAction()   );
   m_pHangup   = createButton( SFLPhone::app()->getHangupAction()   );
   m_pTransfer = createButton( SFLPhone::app()->getTransferAction() );
   m_pRecord   = createButton( SFLPhone::app()->getRecordAction()   );
   m_pRefuse   = createButton( SFLPhone::app()->getRefuseAction()   );
   m_pAccept   = createButton( SFLPhone::app()->getAcceptAction()   );

   m_hButtons[ static_cast<int>(ActionButton::HOLD)     ] = m_pHold    ;
   m_hButtons[ static_cast<int>(ActionButton::UNHOLD)   ] = m_pUnhold  ;
   m_hButtons[ static_cast<int>(ActionButton::PICKUP)   ] = m_pPickup  ;
   m_hButtons[ static_cast<int>(ActionButton::HANGUP)   ] = m_pHangup  ;
   m_hButtons[ static_cast<int>(ActionButton::MUTE)     ] = m_pMute    ;
   m_hButtons[ static_cast<int>(ActionButton::TRANSFER) ] = m_pTransfer;
   m_hButtons[ static_cast<int>(ActionButton::RECORD)   ] = m_pRecord  ;
   m_hButtons[ static_cast<int>(ActionButton::REFUSE)   ] = m_pRefuse  ;
   m_hButtons[ static_cast<int>(ActionButton::ACCEPT)   ] = m_pAccept  ;

   layout->addWidget( m_pHangup   );
   layout->addWidget( m_pTransfer );
   layout->addWidget( m_pMute     );
   layout->addWidget( m_pRecord   );
   layout->addWidget( m_pUnhold   );
   layout->addWidget( m_pHold     );
   layout->addWidget( m_pPickup   );
   layout->addWidget( m_pRefuse   );
   layout->addWidget( m_pAccept   );

   setMinimumSize(100,56);
   if (parent)
      parent->installEventFilter(this);
   setVisible(false);
   hideEvent(nullptr);
} //CallViewOverlayToolbar

///Resize event
void CallViewOverlayToolbar::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);
}

///Repaint event
void CallViewOverlayToolbar::paintEvent(QPaintEvent* event)
{
   Q_UNUSED(event);
   QPainter customPainter(this);
   customPainter.setRenderHint(QPainter::Antialiasing);
   QBrush b = customPainter.brush();
   QPen   p = customPainter.pen();

   b.setColor("black");
   //Use the current style pixel metrics to do as well as possible to guess the right shape
   int margin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
   customPainter.setOpacity(0.5);
   customPainter.setBrush (Qt::black);
   customPainter.setPen   (Qt::transparent);
   customPainter.drawRect (QRect(margin,10,width()-2*margin,height()-10-2*margin - margin)               );
   customPainter.drawPie  (QRect(width()-8-margin,height()-10,8,8),270*16,90*16 );
   customPainter.drawPie  (QRect(margin,height()-10,8,8),180*16,90*16           );
   customPainter.drawRect (QRect(4+margin,height()-4-margin,width()-8-2*margin,4)             );

   m_pLeftRender->render (&customPainter,QRect( margin,0,10,10)                 );
   m_pRightRender->render(&customPainter,QRect( width()-10-margin,0,10,10)        );
} //paintEvent

///Create a toolbar button
ObserverToolButton* CallViewOverlayToolbar::createButton(ExtendedAction* action)
{
   ObserverToolButton* b = new ObserverToolButton(this);
   b->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
   b->setIconSize(QSize(28,28));
   b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   b->setText(action->text());
   b->setShortcut(0);
   b->setStyleSheet("margin-bottom:0px;margin-top:7px;font-weight:bold;background-color:transparent;color:#DDDDDD;padding-top:5px;");
   b->setIcon(action->altIcon());
   connect(action,SIGNAL(textChanged(QString)),b,SLOT(setNewText(QString)));
   connect(b,SIGNAL(clicked()),action,SLOT(trigger()));
   return b;
} //createButton

///Hide or show the toolbar and select visible actions
void CallViewOverlayToolbar::updateState()
{
   const QModelIndex& index = m_pParent->selectionModel()->currentIndex();
   if (index.isValid()) {
      if (!m_pParent->selectionModel()->hasSelection()) {
         m_pParent->selectionModel()->setCurrentIndex(index,QItemSelectionModel::SelectCurrent);
      }
      Call::State state = (Call::State) index.data(Call::Role::CallState).toInt();
      setVisible(true);
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(53);
      char act_counter = 0;
      for (int i = 0;i<9;i++) {
         m_hButtons[ i ]->setVisible(visibility[ static_cast<ActionButton>(i) ][state]);
         act_counter += visibility[ static_cast<ActionButton>(i) ][state];
      }
      if (!act_counter)
         setVisible(false);
   }
   else {
      setVisible(false);
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(0);
   }
   //Now set the top margin, this doesn't really belong anywhere, so why not here
   int rows = CallModel::instance()->rowCount(QModelIndex());
   QModelIndex last = CallModel::instance()->index(rows-1,0);
   if (CallModel::instance()->rowCount(last) > 0)
      last = CallModel::instance()->index(CallModel::instance()->rowCount(last)-1,0,last);
   QRect topMargin =  m_pParent->visualRect(last);
   TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
   manager->setTopMargin(topMargin.y()+topMargin.height());
} //updateState

void CallViewOverlayToolbar::hideEvent(QHideEvent *)
{
   if (parentWidget()->property("tipManager").isValid()) {
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(0);
   }
   emit visibilityChanged(false);
}

void CallViewOverlayToolbar::showEvent(QShowEvent *)
{
   if (parentWidget()->property("tipManager").isValid()) {
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(53);
   }
    emit visibilityChanged(true);
}

bool CallViewOverlayToolbar::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize && parentWidget()) {
      resize(parentWidget()->width(),72);
      move(0,parentWidget()->height()-72);
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}