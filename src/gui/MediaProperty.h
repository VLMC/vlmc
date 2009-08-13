/*****************************************************************************
 * MediaProperty.h: Handle the media property and meta tags edition
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef MEDIAPROPERTY_H
#define MEDIAPROPERTY_H

#include <QDialog>
#include "Clip.h"
#include <QAbstractButton>

namespace Ui {
    class MediaProperty;
}

class MediaProperty : public QDialog
{
    Q_OBJECT
public:
    MediaProperty( Clip* clip, QWidget *parent = 0 );
    ~MediaProperty();

protected:
    void changeEvent(QEvent *e);

private slots:
    void        apply();
    void        addTagsRequired();
    void        removeTagsRequired();

private:
    Ui::MediaProperty*  ui;
    Clip*               m_clip;
};

#endif // MEDIAPROPERTY_H
