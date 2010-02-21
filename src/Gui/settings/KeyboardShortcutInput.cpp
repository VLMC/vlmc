/*****************************************************************************
 * KeyboardShortcutInput.cpp: Input widget for keybard shortcut
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

#include "SettingsManager.h"
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
    QKeySequence    ks( initialValue );
    for ( int i = 0; i < 4; ++i )
        m_shortcuts[i] = ks[i];
}

void    KeyboardShortcutInput::mousePressEvent( QMouseEvent* )
{
    m_capturing = true;
    setDown( true );
    memcpy( m_initialValue, m_shortcuts, 4 * sizeof(int) );
    memset( m_shortcuts, 0, sizeof( m_shortcuts ) );
    m_current = 0;
    setText( "" );
}

void    KeyboardShortcutInput::mouseReleaseEvent( QMouseEvent* )
{
}

QString
KeyboardShortcutInput::shortcut() const
{
    QKeySequence    seq( m_shortcuts[0], m_shortcuts[1], m_shortcuts[2], m_shortcuts[3] );
    return seq.toString();
}

void    KeyboardShortcutInput::release()
{
    QKeySequence    seq( m_shortcuts[0], m_shortcuts[1], m_shortcuts[2], m_shortcuts[3] );

    m_capturing = false;
    setDown( false );
    setText( seq.toString() );
}

void    KeyboardShortcutInput::cancel()
{
    memcpy( m_shortcuts, m_initialValue, 4 * sizeof(int) );
    release();
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
        cancel();
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
