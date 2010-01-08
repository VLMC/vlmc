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

/**
 * \class VLMCSettingsDefault
 *
 * \brief Static class for loading default parameters
 *        for the application
 */

class VLMCSettingsDefault
{
    public:

        /**
         * \brief load all the application defaults parameters
         * \param part the settings part in wich the default values will
         *        be loaded
         */

        static void load( const QString& part );

        /**
         * \brief will load the values for the shortcuts settings.
         *
         * This method will create a new part in the settings Manager
         * called keyboard_shortcut
         * \sa  loadKeyboardShortcutDefaults( const QString& part )
         */

        static void loadKeyboardShortcutDefaults();

    private:

        /**
         * \brief will load the values for the application global values.
         * \param part the settings part in wich the global default
         *        values will be loaded
         */

        static void loadVLMCDefaults( const QString& part );

        /**
         * \brief will load the values for the language settings.
         * \param part the settings part in wich the language
         *        default values will be loaded
         */

        static void loadlanguageDefaults( const QString& part );

        /**
         * \brief will load the values for the shortcuts settings.
         * \param part the settings part in wich the language
         *        default values will be loaded
         */

        static void loadKeyboardShortcutDefaults( const QString& part );

    private:
        VLMCSettingsDefault();
        ~VLMCSettingsDefault();
        VLMCSettingsDefault( const VLMCSettingsDefault& );
        VLMCSettingsDefault&  operator = ( const VLMCSettingsDefault& );
};

#endif
