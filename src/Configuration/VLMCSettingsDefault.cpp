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

#include <QVariant>

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
    QVariant defaultOutputPFS = "30";
    QVariant defaultTrackNb = "64";

    settingsMan->setValue( part,
                           "VLMCOutPutFPS", defaultOutputPFS  );
    settingsMan->setValue( part,
                           "VLMCTracksNb",
                           defaultTrackNb  );

    return ;
}

void    VLMCSettingsDefault::loadlanguageDefaults( const QString& part )
{
    SettingsManager*    setMan = SettingsManager::getInstance();

    QVariant    defaultLang = "fr";
    setMan->setValue( part,
                      "VLMCLang",
                      defaultLang );

    return ;
}
