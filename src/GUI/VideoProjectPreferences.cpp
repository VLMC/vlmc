/*****************************************************************************
 * VideoProjectPreferences.cpp: VLMC video project preferences class
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

#include "VideoProjectPreferences.h"
#include "SettingsManager.h"

#include "QDebug"

VideoProjectPreferences::VideoProjectPreferences( QWidget *parent )
    : PreferenceWidget( parent )
{
	m_ui.setupUi( this );
}

VideoProjectPreferences::~VideoProjectPreferences() { }

void    VideoProjectPreferences::load()
{
//    qDebug() << "Loading Prefs : VideoProject";
//    int projectFps = SettingsManager::getInstance()->getValue( "VideoProjectFPS" ).toInt();
//    int projectHeight = SettingsManager::getInstance()->getValue( "VideoProjectHeight" ).toInt();
//    int projectWidth = SettingsManager::getInstance()->getValue( "VideoProjectWidth" ).toInt();
//
//    m_ui.FPSSpinBox->setValue( projectFps );
//    m_ui.HeightSpinBox->setValue( projectHeight );
//    m_ui.WidthSpinBox->setValue( projectWidth );
}

void    VideoProjectPreferences::save()
{
//    settings.insert( "VideoProjectFPS", QVariant( m_ui.FPSSpinBox->value() ) );
//    settings.insert( "VideoProjectHeight", QVariant( m_ui.HeightSpinBox->value() ) );
//    settings.insert( "VideoProjectWidth", QVariant( m_ui.WidthSpinBox->value() ));
    return ;
}
