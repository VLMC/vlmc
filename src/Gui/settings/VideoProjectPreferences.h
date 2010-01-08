/*****************************************************************************
 * VideoProjectPreferences.h: VLMC video project preferences class
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

#ifndef VIDEOPROJECTPREFERENCES_H
#define VIDEOPROJECTPREFERENCES_H

#include "PreferenceWidget.h"
#include "ui_VideoProjectPreferences.h"

class QWidget;

class VideoProjectPreferences : public PreferenceWidget
{
    //Q_OBJECT
    public:
        VideoProjectPreferences( QWidget *parent = 0 );
        ~VideoProjectPreferences();
        void    load();
        void    save();
    private:
        Ui::VideoProjectPreferences m_ui;
};

#endif // VIDEOPROJECTPREFERENCES_H
