/*****************************************************************************
 * ProjectSettingsDefault.h: Project default Settings class
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

#ifndef PROJECTSETTINGSDEFAULT_H
#define PROJECTSETTINGSDEFAULT_H

#include <QString>

/**
 * \class ProjectSettingsDefault 
 *
 * \brief Static class for loading default parameters
 *        for the project
 */

class ProjectSettingsDefault
{
    public:

        /**
         * \brief load all the project defaults parameters
         * \param part the settings part in wich the default values will
         *        be loaded
         */

        static void load( const QString& part );

    private:

        /**
         * \brief will load the project Audio related values.
         * \param part the settings part in wich the audio default
         *        values will be loaded
         */

        static void loadAudioDefaults( const QString& part );

        /**
         * \brief will load the project Video related values.
         * \param part the settings part in wich the audio default
         *        values will be loaded
         */

        static void loadVideoDefaults( const QString& part );

        /**
         * \brief will load the Project global values.
         * \param part the settings part in wich the project default
         *        values will be loaded
         */

        static void loadProjectDefaults( const QString& part );

    private:
        ProjectSettingsDefault();
        ~ProjectSettingsDefault();
        ProjectSettingsDefault( const ProjectSettingsDefault & );
        ProjectSettingsDefault& operator = ( const ProjectSettingsDefault& );
};

#endif

