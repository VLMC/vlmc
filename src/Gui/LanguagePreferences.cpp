/*****************************************************************************
 * Preferences.cpp: Preferences window
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
 * Authors: Clement CHAVANCE <kinder@vlmcorg>
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
#include <QVariant>

#include "SettingsManager.h"
#include "LanguagePreferences.h"
#include "ui_LanguagePreferences.h"

#include <QDir>

QTranslator* LanguagePreferences::m_currentLang = NULL;

LanguagePreferences::LanguagePreferences( QWidget *parent )
    : PreferenceWidget( parent )
{
    m_ui.setupUi( this );

    QDir            dir( "ts/", "*.qm", QDir::Name | QDir::IgnoreCase, QDir::Files );
    QStringList     tss = dir.entryList();

    m_ui.comboBoxLanguage->setInsertPolicy( QComboBox::InsertAlphabetically );
    foreach ( const QString& tsFileName, tss )
    {
        QString     localeStr;
        int         localePos = tsFileName.lastIndexOf( "vlmc_");
        int         dotPos = tsFileName.lastIndexOf( ".qm" );
        if ( localePos < 0 || dotPos < 0 )
        {
            qWarning() << "Invalid translation file:" << tsFileName;
            continue ;
        }
        localePos += 5;
        localeStr = tsFileName.mid( localePos, dotPos - localePos );
        QLocale     locale( localeStr );
        qDebug() << "Adding new language:" << QLocale::countryToString( locale.country() ) << '/' <<
                QLocale::languageToString( locale.language() ) << "with locale" << localeStr;
        m_ui.comboBoxLanguage->addItem( QLocale::countryToString( locale.country() ) + " / "
                                        + QLocale::languageToString( locale.language() ), localeStr );
    }
    m_ui.comboBoxLanguage->addItem( "UnitedStates / English",    "en_US" );
}

LanguagePreferences::~LanguagePreferences() {}

void LanguagePreferences::load()
{
    const QString& part = m_defaults ? "default" : m_settName;
    SettingsManager         *setMan = SettingsManager::getInstance();
    const SettingValue      *lang = setMan->getValue( part, "VLMCLang" );
    int idx = m_ui.comboBoxLanguage->findData( lang->get() );

    if ( idx != -1 )
        m_ui.comboBoxLanguage->setCurrentIndex( idx );
    return ;
}

void LanguagePreferences::save()
{
    SettingsManager*    setMan = SettingsManager::getInstance();
    QVariant lang = m_ui.comboBoxLanguage->itemData( m_ui.comboBoxLanguage->currentIndex() );

    setMan->setValue( m_settName, "VLMCLang", lang );
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
        m_currentLang->load( langValue, ":/Lang/" );
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
