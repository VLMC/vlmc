/*****************************************************************************
 * Languagehelper.cpp: Watch for language changes
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

#include "LanguageHelper.h"

#include <QApplication>
#include <QTranslator>
#include <QVariant>

#define TS_PREFIX "vlmc_"

LanguageHelper::LanguageHelper() : m_translator( NULL )
{
    connect( qApp, SIGNAL( aboutToQuit() ), this, SLOT( deleteLater() ) );
}

LanguageHelper::~LanguageHelper()
{
    if ( m_translator )
        delete m_translator;
}

void
LanguageHelper::languageChanged( const QVariant &vLang )
{
    QString     lang = vLang.toString();
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
