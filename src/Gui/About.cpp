/*****************************************************************************
 * About.cpp: About dialog
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include <QApplication>
#include <QFile>
#include <QString>
#include <QPlainTextEdit>
#include "About.h"

About* About::m_instance = NULL;

About::About( QWidget *parent ) :
    QDialog( parent )
{
    m_ui.setupUi( this );
    m_ui.tabWidget->setCurrentIndex( 0 );
    setText( ":/text/AUTHORS", m_ui.plainTextEditAuthors );
    setText( ":/text/TRANSLATORS", m_ui.plainTextEditTranslators );
    setText( ":/text/COPYING", m_ui.plainTextEditLicense );
    m_ui.labelVersion->setText(
            m_ui.labelVersion->text().arg( qApp->applicationVersion() )
            );
    connect( qApp, SIGNAL( aboutToQuit() ), this, SLOT( deleteLater() ) );
}

void About::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        break;
    }
}

About* About::instance()
{
    if ( m_instance )
        return m_instance;
    m_instance = new About();
    return m_instance;
}

void About::setText( const QString& filename, QPlainTextEdit* widget )
{
    QFile file( filename );
    if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) )
        widget->insertPlainText( QString::fromUtf8( file.readAll() ) );
    widget->moveCursor( QTextCursor::Start );
}
