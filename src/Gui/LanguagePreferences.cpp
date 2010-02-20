/*****************************************************************************
 * Preferences.cpp: Preferences window
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
 *          Clement CHAVANCE <kinder@vlmc.org>
 *          Hugo Beauzee-Luyssen <beauze.h@gmail.com>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#include "SettingsManager.h"
#include "LanguagePreferences.h"
#include "ui_LanguagePreferences.h"
#include "SettingValue.h"

#include <QVariant>
#include <QDir>
#include <QLocale>

QTranslator* LanguagePreferences::m_currentLang = NULL;

LanguagePreferences::LanguagePreferences( QWidget *parent )
    : PreferenceWidget( parent ),
    m_type( SettingsManager::Vlmc )
{
    m_ui.setupUi( this );

    QDir dir( ":/ts/", "*.qm", QDir::Name | QDir::IgnoreCase, QDir::Files );

    foreach ( const QString& tsFileName, dir.entryList() )
    {
        QString     countryCode;
        int         localePos = tsFileName.lastIndexOf( TS_PREFIX );
        int         extPos = tsFileName.lastIndexOf( ".qm" );

        if ( localePos < 0 || extPos < 0 || localePos > extPos )
        {
            qWarning() << "Invalid translation file:" << tsFileName;
            continue ;
        }

        localePos += qstrlen( TS_PREFIX );
        countryCode = tsFileName.mid( localePos, extPos - localePos );
        QLocale     locale( countryCode );

        // Check if the country code is valid ISO 639
        if ( locale.language() == QLocale::C )
        {
            qWarning() << "Invalid locale for file:" << tsFileName;
            continue;
        }

        m_ui.comboBoxLanguage->addItem( QString( "%1 (%2)" ).arg(
                QLocale::languageToString( locale.language() ),
                QLocale::countryToString( locale.country() ) ), countryCode );
    }
    m_ui.comboBoxLanguage->addItem( "English (UnitedStates)",    "en_US" );

    // Sort the combobox
    m_ui.comboBoxLanguage->model()->sort( 0 );
    VLMC_CREATE_PREFERENCE( "global/VLMCLang", "en_US", "The VLMC's UI language" );
}

LanguagePreferences::~LanguagePreferences() {}

void LanguagePreferences::load()
{
    SettingsManager         *setMan = SettingsManager::getInstance();

    QVariant      lang = setMan->value( "global/VLMCLang", "en_US", m_type )->get();
    int idx = m_ui.comboBoxLanguage->findData( lang );

    if ( idx != -1 )
        m_ui.comboBoxLanguage->setCurrentIndex( idx );
    return ;
}

void LanguagePreferences::save()
{
    SettingsManager*    setMan = SettingsManager::getInstance();
    QVariant lang = m_ui.comboBoxLanguage->itemData( m_ui.comboBoxLanguage->currentIndex() );

    setMan->setImmediateValue( "VLMCLang", lang, m_type );
    changeLang( lang.toString() );
}

void LanguagePreferences::changeLang( QString langValue )
{
    if ( m_currentLang != NULL )
    {
        qApp->removeTranslator( m_currentLang );
        delete m_currentLang;
        m_currentLang = NULL;
    }
    if ( !langValue.isEmpty() )
    {
        m_currentLang = new QTranslator();
        m_currentLang->load( TS_PREFIX + langValue, ":/ts/" );
        qApp->installTranslator( m_currentLang );
    }
}

void LanguagePreferences::changeEvent( QEvent *e )
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
