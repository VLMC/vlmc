/*****************************************************************************
 * AudioProjectPreferences.h: VLMC Audio project preferences class
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#ifndef AUDIOPROJECTPREFERENCES_H
#define AUDIOPROJECTPREFERENCES_H

#include "PreferenceWidget.h"
#include "SettingsManager.h"
#include "ui_AudioProjectPreferences.h"


class QWidget;

class AudioProjectPreferences : public PreferenceWidget
{
    //Q_OBJECT
    public:
        AudioProjectPreferences( QWidget *parent = 0 );
        ~AudioProjectPreferences();
        void    load();
        void    save();
    protected:
        virtual void changeEvent( QEvent *e );
    private:
        Ui::AudioProjectPreferences m_ui;
        const SettingsManager::Type m_type;
};

#endif // AUDIOPROJECTPREFERENCES_H
