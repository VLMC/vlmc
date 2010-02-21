/*****************************************************************************
 * LanguageWidget.h: Handle languge settings
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@vlmc.org>
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

#include "config.h"
#include "LanguageWidget.h"
#include "SettingsManager.h"
#include "SettingValue.h"

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QTranslator>
#include <QtDebug>

#define TS_PREFIX "vlmc_"

LanguageWidget::LanguageWidget( SettingValue *s, QWidget *parent /*= NULL*/ ) :
        m_setting( s )
{
    m_list = new QComboBox( parent );
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

        m_list->addItem( QString( "%1 (%2)" ).arg(
                QLocale::languageToString( locale.language() ),
                QLocale::countryToString( locale.country() ) ), countryCode );
    }
    m_list->addItem( "English (UnitedStates)",    "en_US" );

    // Sort the combobox
    m_list->model()->sort( 0 );
    QString lang = VLMC_GET_STRING( "general/VLMCLang" );
    int idx = m_list->findData( lang );
    if ( idx != -1 )
        m_list->setCurrentIndex( idx );
}

QWidget*
LanguageWidget::widget()
{
    return m_list;
}

void
LanguageWidget::save()
{
    QString     lang = m_list->itemData( m_list->currentIndex() ).toString();
    m_setting->set( lang );

    if ( !lang.isEmpty() )
    {
        if ( m_translator != NULL )
        {
            qApp->removeTranslator( m_translator );
            delete m_translator;
            m_translator = NULL;
        }
        m_translator = new QTranslator();
        m_translator->load( TS_PREFIX + lang, ":/ts/" );
        qApp->installTranslator( m_translator );
    }
}
