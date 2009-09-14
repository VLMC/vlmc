/*****************************************************************************
 * AudioProjectPreferences.cpp: VLMC Audio project preferences class
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

#include "AudioProjectPreferences.h"

#include "QDebug"

AudioProjectPreferences::AudioProjectPreferences( QWidget *parent )
        : PreferenceWidget( parent )
{
    setWidget( this );
    m_ui.setupUi( this );
}

AudioProjectPreferences::~AudioProjectPreferences() { }

bool    AudioProjectPreferences::load()
{
    qDebug() << "Loading AudioProjectPreferences values";
    return true;
}

void    AudioProjectPreferences::save( QSettings& settings )
{
    settings.beginGroup( "Project" );
    //settings.setValue( "outputFPS", m_ui.outputFPS->text() );
    settings.endGroup();
}
