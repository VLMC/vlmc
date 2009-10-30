/*****************************************************************************
 * VLMCPreferences.cpp: VLMC preferences class
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

#include "VLMCPreferences.h"
#include "SettingsManager.h"

#include "QDebug"

VLMCPreferences::VLMCPreferences( QWidget *parent )
        : PreferenceWidget( parent )
{
    m_ui.setupUi(this);
}

VLMCPreferences::~VLMCPreferences() { }

void    VLMCPreferences::load()
{
//    qDebug() << "Loading Prefs : VLMC";
//    QString  outputFPS = SettingsManager::getInstance()->getValue( "VLMCOutPutFPS" ).toString();
//    QString  previewFPS = SettingsManager::getInstance()->getValue( "VLMCPreviewFPS" ).toString();
//    QString  tracksNb = SettingsManager::getInstance()->getValue( "VLMCTracksNb" ).toString();
//
//    m_ui.outputFPS->setText( outputFPS );
//    m_ui.previewFPS->setText( previewFPS );
//    m_ui.tracksNb->setText( tracksNb );
//
}

void    VLMCPreferences::save( QHash<QString, QVariant>& settings )
{
//    settings.insert( "VLMCOutPutFPS", QVariant( m_ui.outputFPS->text() ) );
//    settings.insert( "VLMCPreviewFPS", QVariant( m_ui.previewFPS->text() ) );
//    settings.insert( "VLMCTracksNb", QVariant( m_ui.tracksNb->text() ) );
}
