/*****************************************************************************
 * ProjectSettingsDefault.cpp: Project default Settings class
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

#include "SettingsManager.h"
#include "ProjectSettingsDefault.h"
#include "ProjectManager.h"

void    ProjectSettingsDefault::load( const QString& part )
{
    SettingsManager::getInstance()->addNewSettingsPart( part );
    ProjectSettingsDefault::loadAudioDefaults( part );
    ProjectSettingsDefault::loadVideoDefaults( part );
    ProjectSettingsDefault::loadProjectDefaults( part );
    SettingsManager::getInstance()->commit();

    return ;
}

void    ProjectSettingsDefault::loadAudioDefaults( const QString& part )
{
    SettingsManager* setMan = SettingsManager::getInstance();
    QVariant defaultSampleRate = 0;
    setMan->setValue( part, "AudioSampleRate", defaultSampleRate );

    return ;
}

void    ProjectSettingsDefault::loadVideoDefaults( const QString& part )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();
    QVariant defaultProjectFPS = 30;
    QVariant defaultProjectHeight = 800;
    QVariant defaultProjectWidth = 600;

    settingsMan->setValue( part,
                           "VideoProjectFPS",
                           defaultProjectFPS );
    settingsMan->setValue( part,
                           "VideoProjectHeight",
                           defaultProjectHeight );
    settingsMan->setValue( part,
                           "VideoProjectWidth",
                           defaultProjectWidth );

    return ;
}

void    ProjectSettingsDefault::loadProjectDefaults( const QString& part )
{
    SettingsManager*  setMan = SettingsManager::getInstance();
    setMan->setValue( part, "ProjectName", ProjectManager::unNamedProject );

    return ;
}

