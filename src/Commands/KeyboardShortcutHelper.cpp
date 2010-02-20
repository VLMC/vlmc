/*****************************************************************************
 * KeyboardShortcutHelper.cpp: An helper to catch keyboard shortcut modifications
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzee-Luyssen <beauze.h@gmail.com>
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

#include <QtDebug>
#include <QAction>

#include "SettingValue.h"
#include "SettingsManager.h"
#include "KeyboardShortcutHelper.h"

KeyboardShortcutHelper::KeyboardShortcutHelper( const QString& name, QWidget* parent ) :
        QShortcut( parent ),
        m_name( name ),
        m_action( NULL )
{
    QString set = VLMC_GET_STRING( name );
    setKey( QKeySequence( set ) );
    SettingsManager::getInstance()->watchValue( name, this,
                                                SLOT( shortcutUpdated( const QVariant& ) ),
                                                SettingsManager::Vlmc );
}

KeyboardShortcutHelper::KeyboardShortcutHelper( const QString& name, QAction *action,
                                                QWidget *parent /*= NULL*/ ) :
    QShortcut( parent ),
    m_name( name ),
    m_action( action )
{

}

void    KeyboardShortcutHelper::shortcutUpdated( const QVariant& value )
{
    if ( m_action == NULL )
        setKey( QKeySequence( value.toString() ) );
    else
        m_action->setShortcut( value.toString() );
}
