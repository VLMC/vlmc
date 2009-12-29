/*****************************************************************************
 * UndoStack.cpp: UndoStack For Undo/Redo Purposes
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
 *          Hugo Beauzee-Luyssen <beauze.h@gmail.com>
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

#include <QUndoStack>
#include <QUndoCommand>

#include "UndoStack.h"
#include "ProjectManager.h"
#include "SettingsManager.h"
#include "KeyboardShortcutHelper.h"

UndoStack::UndoStack( QWidget* parent ) : QUndoView( parent )
{
    setEmptyLabel( tr( "Nothing to undo" ) );

    m_undoStack = new QUndoStack( this );
    setStack( m_undoStack );
    connect( m_undoStack, SIGNAL( cleanChanged( bool ) ),
             ProjectManager::getInstance(), SLOT( cleanChanged( bool ) ) );
    connect( ProjectManager::getInstance(), SIGNAL( projectSaved() ),
             m_undoStack, SLOT( setClean() ) );

    m_undoShortcut = new KeyboardShortcutHelper( "Undo", this );
    m_redoShortcut = new KeyboardShortcutHelper( "Redo", this );
    connect( m_undoShortcut, SIGNAL( activated() ), m_undoStack, SLOT( undo() ) );
    connect( m_redoShortcut, SIGNAL( activated() ), m_undoStack, SLOT( redo() ) );

    connect( ProjectManager::getInstance(), SIGNAL( projectClosed() ), this, SLOT( clear() ) );
}

void    UndoStack::push( QUndoCommand* command )
{
    m_undoStack->push( command );
}

void    UndoStack::clear()
{
    m_undoStack->clear();
}
