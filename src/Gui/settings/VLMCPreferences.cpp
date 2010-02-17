/*****************************************************************************
 * VLMCPreferences.cpp: VLMC preferences class
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

#include "VLMCPreferences.h"
#include "SettingsManager.h"

#include <QWidget>
#include <QtDebug>

VLMCPreferences::VLMCPreferences( QWidget *parent )
    : PreferenceWidget( parent ),
    m_type( SettingsManager::QSett )
{
    m_ui.setupUi(this);
    setAutomaticSaveLabelVisiblity( m_ui.automaticSave->isChecked() );
    connect( m_ui.automaticSave, SIGNAL( stateChanged(int) ), this, SLOT( setAutomaticSaveLabelVisiblity( int ) ) );
}

VLMCPreferences::~VLMCPreferences()
{
}

void    VLMCPreferences::setAutomaticSaveLabelVisiblity( int visible )
{
    setAutomaticSaveLabelVisiblity( visible != 0 );
}

void    VLMCPreferences::setAutomaticSaveLabelVisiblity( bool visible )
{
    m_ui.automaticSaveInterval->setVisible( visible );
    m_ui.automaticSaveIntervalLabel->setVisible( visible );
    m_ui.minutesLabel->setVisible( visible );
}

void    VLMCPreferences::load()
{
    SettingsManager* settMan = SettingsManager::getInstance();

    QString     outputFPS = settMan->value( "global/VLMCOutPutFPS", "30", m_type ).toString();
    QString     tracksNb = settMan->value( "global/VLMCTracksNb", "64", m_type ).toString();
    bool        autoSave = settMan->value( "global/AutomaticBackup", false, m_type ).toBool();
    QString     autoSaveInterval = settMan->value( "global/AutomaticBackupInterval", "5", m_type ).toString();

    m_ui.outputFPS->setText( outputFPS );
    m_ui.tracksNb->setText( tracksNb );
    m_ui.automaticSave->setChecked( autoSave );
    m_ui.automaticSaveInterval->setText( autoSaveInterval );
    setAutomaticSaveLabelVisiblity( autoSave );
}

void    VLMCPreferences::save()
{
    SettingsManager* settMan = SettingsManager::getInstance();
    QVariant outputFPS( m_ui.outputFPS->text() );
    QVariant tracksNb( m_ui.tracksNb->text() );
    QVariant autoSave( m_ui.automaticSave->isChecked() );
    QVariant autoSaveInterval( m_ui.automaticSaveInterval->text() );

    settMan->setImmediateValue( "global/VLMCOutPutFPS", outputFPS, m_type );
    settMan->setImmediateValue( "global/VLMCTracksNb", tracksNb, m_type );
    settMan->setImmediateValue( "global/AutomaticBackup", autoSave, m_type );
    settMan->setImmediateValue( "global/AutomaticBackupInterval", autoSaveInterval, m_type );
}

void VLMCPreferences::changeEvent( QEvent *e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        break;
    }
}
