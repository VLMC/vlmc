/*****************************************************************************
 * KeyboardShortcut.cpp: Keyboard shortcut preferences
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

#include "KeyboardShortcut.h"
#include "KeyboardShortcutInput.h"
#include "SettingsManager.h"

#include <QtDebug>

KeyboardShortcut::KeyboardShortcut( QWidget* parent )
    : PreferenceWidget( parent ),
    m_type( SettingsManager::Vlmc )
{
    m_layout = new QFormLayout( this );
    const QHash<QString, QVariant>    shortcutGroup = SettingsManager::getInstance()->group( "keyboard", SettingsManager::Vlmc );
    Q_ASSERT( !shortcutGroup.size() == 0 );

    QHash<QString, QVariant>::const_iterator    it = shortcutGroup.begin();
    QHash<QString, QVariant>::const_iterator    ite = shortcutGroup.end();
    while ( it != ite )
    {
        m_keySeq[it.key()] = new QKeySequence( it.value().toString() );
        KeyboardShortcutInput*  ksi = new KeyboardShortcutInput( it.key(), m_keySeq[it.key()]->toString(), this );
        m_layout->addRow( it.key(), ksi );
        connect( ksi, SIGNAL( changed( const QString&, const QString& ) ),
                 this, SLOT( shortcutUpdated( const QString&, const QString& ) ) );
        ++it;
    }
    setLayout( m_layout );
}

KeyboardShortcut::~KeyboardShortcut()
{
}

void        KeyboardShortcut::load()
{
}

void        KeyboardShortcut::save()
{
}

void        KeyboardShortcut::shortcutUpdated( const QString& name, const QString& value )
{
    SettingsManager::getInstance()->setImmediateValue( "keyboard/" + name, value, m_type );
}
