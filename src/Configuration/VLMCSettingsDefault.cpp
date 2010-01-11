/*****************************************************************************
 * VLMCSettingsDefault.cpp: VLMC default Settings class
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
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
#include <QVariant>
#include <QDir>

#include "VLMCSettingsDefault.h"
#include "SettingsManager.h"

void    VLMCSettingsDefault::load( const QString& part )
{
    SettingsManager::getInstance()->addNewSettingsPart( part );
    VLMCSettingsDefault::loadVLMCDefaults( part );
    VLMCSettingsDefault::loadlanguageDefaults( part );
    SettingsManager::getInstance()->commit();
    return ;
}

void    VLMCSettingsDefault::loadVLMCDefaults( const QString& part )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();

    settingsMan->setValue( part,
                           "VLMCOutPutFPS", "30" );
    settingsMan->setValue( part,
                           "VLMCTracksNb",
                           "64" );
    settingsMan->setValue( part,
                           "VLMCWorkspace",
                           QDir::homePath() );
    settingsMan->setValue( part, "AutomaticBackup", false );
    settingsMan->setValue( part, "AutomaticBackupInterval", 5 );

    settingsMan->setValue( "private", "LogLevel", QtWarningMsg );
    settingsMan->setValue( "private", "LogFile", "vlmc.log" );

    return ;
}

void    VLMCSettingsDefault::loadlanguageDefaults( const QString& part )
{
    SettingsManager*    setMan = SettingsManager::getInstance();

    setMan->setValue( part, "VLMCLang", "en_US" );
    return ;
}

void    VLMCSettingsDefault::loadKeyboardShortcutDefaults()
{
    SettingsManager::getInstance()->addNewSettingsPart( "keyboard_shortcut" );
    loadKeyboardShortcutDefaults( "default" );
    loadKeyboardShortcutDefaults( "keyboard_shortcut" );
    SettingsManager::getInstance()->commit();
}

#define ADD_SHORTCUT(NAME, KEYS)    setMan->setValue( part, QObject::tr( NAME ), QVariant( QObject::tr( KEYS ) ) )

void    VLMCSettingsDefault::loadKeyboardShortcutDefaults( const QString& part )
{
    SettingsManager*    setMan = SettingsManager::getInstance();

    ADD_SHORTCUT( "Default mode", "n" );
    ADD_SHORTCUT( "Cut mode", "x" );
    ADD_SHORTCUT( "Launch media preview", "Ctrl+Return" );
    ADD_SHORTCUT( "Start render preview", "Space" );
    //A bit nasty, but we better use what Qt's providing as default shortcut
    ADD_SHORTCUT( "Undo", QKeySequence( QKeySequence::Undo ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Redo", QKeySequence( QKeySequence::Redo ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Help", QKeySequence( QKeySequence::HelpContents ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Quit", "Ctrl+Q" );
    ADD_SHORTCUT( "Preferences", "Alt+P" );
    ADD_SHORTCUT( "Fullscreen", "F" );
    ADD_SHORTCUT( "New project", QKeySequence( QKeySequence::New ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Open project", QKeySequence( QKeySequence::Open ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Save", QKeySequence( QKeySequence::Save ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Save as", "Ctrl+Shift+S" );
    ADD_SHORTCUT( "Close project", QKeySequence( QKeySequence::Close ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Import media", "Ctrl+I" );
    return ;
}

#undef ADD_SHORTCUT
