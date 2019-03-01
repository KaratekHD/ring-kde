/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
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
#include "contactphoto.h"

#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtGui/QColor>
#include <QtGui/QPalette>

#include <contactmethod.h>
#include <individual.h>
#include <person.h>

// KDE
#include <KColorScheme>

class ContactPhotoPrivate final : public QObject
{
    Q_OBJECT
public:
    ContactMethod* m_pContactMethod {nullptr};
    Person* m_pPerson {nullptr};
    Person* m_pCurrentPerson {nullptr};
    bool m_DisplayEmpty {true};
    bool m_DrawEmptyOutline {true};
    QVariant m_DefaultColor {};
    QSharedPointer<Event> m_pEvent {nullptr};

    enum class TrackingStatus {
        AUTO     = 0,
        DISABLED = 1,
        ENABLED  = 2
    } m_TrackingStatus {TrackingStatus::AUTO};

    ContactPhoto* q_ptr;
public Q_SLOTS:
    void slotPhotoChanged();
    void slotContactChanged(Person* newContact, Person* oldContact);
    void slotPresenceChanged();
};

ContactPhoto::ContactPhoto(QQuickItem* parent) : QQuickPaintedItem(parent),
    d_ptr(new ContactPhotoPrivate)
{
    d_ptr->q_ptr = this;
}

ContactPhoto::~ContactPhoto()
{
    delete d_ptr;
}

void ContactPhoto::paint(QPainter *painter)
{
    const qreal w = width ();
    const qreal h = height();
    const qreal s = std::min(w, h);

    QPainterPath imageClip, cornerPath;
    static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText )
        .brush(QPalette::Normal).color();
    static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText )
        .brush(QPalette::Normal).color();

    Person* p = d_ptr->m_pPerson;

    if ((!p) && d_ptr->m_pContactMethod) {
        p = d_ptr->m_pContactMethod->contact();
    }

    const bool tracked = isTracked();
    const bool present = isPresent();

// for tests
//     const bool isTracked(true), isPresent(rand()%2);

    imageClip.addEllipse(3, 3, s-6, s-6);

    cornerPath.moveTo(s/2, s);
    cornerPath.arcTo(0, 0, s, s, 270, 90);
    cornerPath.lineTo(s,s-6);
    cornerPath.arcTo(s-12, s-12, 12, 12, 0, -90);
    cornerPath.lineTo(s/2, s);

    painter->setRenderHint(QPainter::Antialiasing);

    auto pen = painter->pen();

    auto color = d_ptr->m_DefaultColor.value<QColor>();

    if (color.isValid()) //TODO C++17
        pen.setColor(color);
    else
        pen.setColor(QGuiApplication::palette().text().color());

    pen.setWidthF(1.5);

    painter->setPen(pen);

    // Center the image
    if (w > s)
        painter->translate((w-s)/2, 0);

    if (p && hasPhoto()) {
        painter->drawEllipse(QRectF {1.5, 1.5, s-1.5, s-1.5});

        painter->setClipPath(imageClip);

        const QPixmap original(qvariant_cast<QPixmap>(p->photo()));

        painter->drawPixmap(3,3,s-6,s-6, original);

        painter->setClipping(false);

        if (tracked) {
            pen.setWidthF(1);
            painter->setPen(pen);
            painter->setBrush(pen.color());
            painter->drawPath(cornerPath);

            painter->setBrush(present ? presentBrush : awayBrush);

            painter->drawEllipse(s-12, s-12, 12, 12);
        }
    }
    else if (d_ptr->m_DisplayEmpty) {
        if (d_ptr->m_DrawEmptyOutline)
            painter->drawEllipse(QRectF {1.5, 1.5, s-1.5, s-1.5});

        painter->setClipPath(imageClip);

        if (tracked)
            pen.setColor(present ? presentBrush : awayBrush);

        painter->setBrush({});
        painter->setPen(pen);

        const qreal headSize = (s/2) / 2;
        const qreal bodySize = headSize*2.5;

        painter->drawEllipse(s/2-headSize/2, headSize, headSize, headSize);
        painter->drawEllipse(s/2-bodySize/2, bodySize, bodySize, bodySize);
    }
}

ContactMethod* ContactPhoto::contactMethod() const
{
    return d_ptr->m_pContactMethod;
}

void ContactPhoto::setContactMethod(ContactMethod* cm)
{
    if (d_ptr->m_pPerson) {
        disconnect(d_ptr->m_pPerson, &Person::photoChanged,
            d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
        d_ptr->m_pPerson = nullptr;
    }

    if (d_ptr->m_pContactMethod) {
        disconnect(d_ptr->m_pContactMethod, &ContactMethod::contactChanged,
            d_ptr, &ContactPhotoPrivate::slotContactChanged);
        if (d_ptr->m_pContactMethod->contact())
            disconnect(d_ptr->m_pContactMethod->contact(), &Person::photoChanged,
                d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
    }

    d_ptr->m_pContactMethod = cm;

    d_ptr->slotContactChanged(cm ? cm->contact() : nullptr, nullptr);

    emit hasPhotoChanged();

    update();
}

Person* ContactPhoto::person() const
{
    return d_ptr->m_pPerson;
}

void ContactPhoto::setPerson(Person* p)
{
    if (d_ptr->m_pPerson) {
        disconnect(d_ptr->m_pPerson, &Person::photoChanged,
            d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
        disconnect(d_ptr->m_pPerson, &Person::presenceChanged,
            d_ptr, &ContactPhotoPrivate::slotPresenceChanged);
    }

    if (d_ptr->m_pCurrentPerson) {
        disconnect(d_ptr->m_pCurrentPerson, &Person::photoChanged,
            d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
        disconnect(d_ptr->m_pCurrentPerson, &Person::presenceChanged,
            d_ptr, &ContactPhotoPrivate::slotPresenceChanged);
        disconnect(d_ptr->m_pContactMethod, &ContactMethod::contactChanged,
            d_ptr, &ContactPhotoPrivate::slotContactChanged);

        if (d_ptr->m_pContactMethod && d_ptr->m_pContactMethod->contact()) {
            disconnect(d_ptr->m_pContactMethod->contact(), &Person::photoChanged,
                d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
        }

        d_ptr->m_pCurrentPerson = nullptr;
    }

    d_ptr->m_pPerson = p;

    connect(d_ptr->m_pPerson, &Person::photoChanged,
        d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
    connect(d_ptr->m_pPerson, &Person::presenceChanged,
        d_ptr, &ContactPhotoPrivate::slotPresenceChanged);

    update();

    emit hasPhotoChanged();
}

Individual* ContactPhoto::individual() const
{
    if (d_ptr->m_pPerson)
        return d_ptr->m_pPerson->individual();

    if (d_ptr->m_pCurrentPerson)
        return d_ptr->m_pCurrentPerson->individual();

    if (d_ptr->m_pContactMethod)
        return d_ptr->m_pContactMethod->individual();

    return {};
}

void ContactPhoto::setIndividual(Individual* ind)
{
    if (!ind) {
        d_ptr->m_pPerson = nullptr;
        d_ptr->m_pContactMethod = nullptr;
    }
    else if (auto p = ind->person()) {
        setPerson(p);
        d_ptr->slotContactChanged(p , d_ptr->m_pCurrentPerson);
    }
    else {
        setContactMethod(ind->lastUsedContactMethod()); //INCORECT
    }

    update();
    emit hasPhotoChanged();
    emit changed();
}

void ContactPhotoPrivate::slotPhotoChanged()
{
    emit q_ptr->hasPhotoChanged();

    q_ptr->update();
}

void ContactPhotoPrivate::slotContactChanged(Person* newContact, Person* oldContact)
{
    Q_UNUSED(newContact)

    if (oldContact)
        disconnect(oldContact, &Person::photoChanged,
            this, &ContactPhotoPrivate::slotPhotoChanged);

    if (m_pCurrentPerson) {
        disconnect(m_pCurrentPerson, &Person::photoChanged,
            this, &ContactPhotoPrivate::slotPhotoChanged);
        disconnect(m_pCurrentPerson, &Person::presenceChanged,
            this, &ContactPhotoPrivate::slotPresenceChanged);
    }

    m_pCurrentPerson = m_pContactMethod ?
        m_pContactMethod->contact() : m_pCurrentPerson;

    if (m_pCurrentPerson && m_pContactMethod) {
        connect(m_pContactMethod, &ContactMethod::contactChanged,
            this, &ContactPhotoPrivate::slotContactChanged);
        connect(m_pContactMethod, &ContactMethod::presentChanged,
            this, &ContactPhotoPrivate::slotPresenceChanged);
        connect(m_pContactMethod, &ContactMethod::trackedChanged,
            this, &ContactPhotoPrivate::slotPresenceChanged);
        connect(m_pCurrentPerson, &Person::photoChanged,
            this, &ContactPhotoPrivate::slotPhotoChanged);
    }
}

void ContactPhotoPrivate::slotPresenceChanged()
{
    //
}

bool ContactPhoto::hasPhoto() const
{
    if (d_ptr->m_pPerson)
        return !d_ptr->m_pPerson->photo().isNull();

    if (d_ptr->m_pCurrentPerson)
        return !d_ptr->m_pCurrentPerson->photo().isNull();

    return false;
}

bool ContactPhoto::displayEmpty() const
{
    return d_ptr->m_DisplayEmpty;
}

void ContactPhoto::setDisplayEmpty(bool val)
{
    d_ptr->m_DisplayEmpty = val;
    update();
}

bool ContactPhoto::drawEmptyOutline() const
{
    return d_ptr->m_DrawEmptyOutline;
}

void ContactPhoto::setDrawEmptyOutline(bool val)
{
    d_ptr->m_DrawEmptyOutline = val;
}

bool ContactPhoto::isTracked() const
{
    if (d_ptr->m_TrackingStatus != ContactPhotoPrivate::TrackingStatus::AUTO)
        return d_ptr->m_TrackingStatus == ContactPhotoPrivate::TrackingStatus::ENABLED;

    if (d_ptr->m_pPerson)
        return d_ptr->m_pPerson->isTracked();

    if (d_ptr->m_pCurrentPerson) {
        if (d_ptr->m_pContactMethod)
            if (auto p = d_ptr->m_pContactMethod->contact())
                return p->isTracked();

        return d_ptr->m_pCurrentPerson->isPresent();
    }

    return false;
}

void ContactPhoto::setTracked(bool t)
{
    d_ptr->m_TrackingStatus = t ?
        ContactPhotoPrivate::TrackingStatus::ENABLED :
        ContactPhotoPrivate::TrackingStatus::DISABLED;

    update();
    emit changed();
}

bool ContactPhoto::isPresent() const
{
    if (d_ptr->m_pPerson)
        return d_ptr->m_pPerson->isPresent();

    if (d_ptr->m_pCurrentPerson) {
        if (d_ptr->m_pContactMethod)
            if (auto p = d_ptr->m_pContactMethod->contact())
                return p->isPresent();

        return d_ptr->m_pCurrentPerson->isPresent();
    }

    return false;
}

QVariant ContactPhoto::defaultColor() const
{
    return d_ptr->m_DefaultColor;
}

void ContactPhoto::setDefaultColor(const QVariant& color)
{
    d_ptr->m_DefaultColor = color;
    update();
}

Event* ContactPhoto::event() const
{
    return d_ptr->m_pEvent ? d_ptr->m_pEvent.data() : nullptr;
}

void ContactPhoto::setEvent(Event* e)
{
    d_ptr->m_pEvent = e->ref();
}

#include <contactphoto.moc>
