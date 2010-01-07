/*****************************************************************************
 * VLMCSettingsDefault.h: VLMC default Settings class
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

#ifndef VLMCSETTINGSDEFAULT_H
#define VLMCSETTINGSDEFAULT_H

#include <QString>

class VLMCSettingsDefault
{
    public:
        static void load( const QString& part );
        static void loadKeyboardShortcutDefaults();

    private:
        static void loadVLMCDefaults( const QString& part );
        static void loadlanguageDefaults( const QString& part );
        static void loadKeyboardShortcutDefaults( const QString& part );
    private:
        VLMCSettingsDefault();
        ~VLMCSettingsDefault();
        VLMCSettingsDefault( const VLMCSettingsDefault& );
        VLMCSettingsDefault&  operator = ( const VLMCSettingsDefault& );
};

#endif
