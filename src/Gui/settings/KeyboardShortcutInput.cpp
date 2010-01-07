/*****************************************************************************
 * KeyboardShortcutInput.cpp: Input widget for keybard shortcut
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#include <QKeyEvent>
#include <QKeySequence>
#include <QTimer>
#include <QtDebug>

#include "KeyboardShortcutInput.h"

KeyboardShortcutInput::KeyboardShortcutInput( const QString& name,
                                              const QString& initialValue, QWidget* parent ) :
        QPushButton( parent ),
        m_name( name ),
        m_capturing( false ),
        m_current( 0 )
{
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( timeout() ) );
    setText( initialValue );
}

void    KeyboardShortcutInput::mousePressEvent( QMouseEvent* )
{
    m_capturing = true;
    setDown( true );
    memset( m_shortcuts, 0, sizeof( m_shortcuts ) );
    m_current = 0;
    setText( "" );
}

void    KeyboardShortcutInput::mouseReleaseEvent( QMouseEvent* )
{
}

void    KeyboardShortcutInput::release()
{
    QKeySequence    seq( m_shortcuts[0], m_shortcuts[1], m_shortcuts[2], m_shortcuts[3] );

    m_capturing = false;
    setDown( false );
    emit changed( m_name, seq.toString() );
}

void    KeyboardShortcutInput::keyPressEvent( QKeyEvent* e )
{
    if ( m_capturing == false )
    {
        QPushButton::keyPressEvent( e );
        return ;
    }
    if ( e->modifiers() == Qt::NoModifier && e->key() == Qt::Key_Escape )
    {
        release();
    }
    else
    {
        m_timer->stop();
        int key = e->key();
        if ( key == Qt::Key_Control || key == Qt::Key_Meta ||
             key == Qt::Key_Shift || key == Qt::Key_Alt ||
             key == Qt::Key_AltGr )
            key = 0;
        int res = key | e->modifiers();
        m_shortcuts[m_current] = res;
        QKeySequence    seq( m_shortcuts[0], m_shortcuts[1], m_shortcuts[2], m_shortcuts[3] );
        setText( seq.toString() );
        if ( key != 0 )
        {
            m_timer->start( 500 );
            ++m_current;
        }
    }
}

void    KeyboardShortcutInput::timeout()
{
    release();
}
