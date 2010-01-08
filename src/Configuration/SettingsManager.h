/*****************************************************************************
 * SettingsManager.h: Backend settings manager
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

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "SettingValue.h"
#include "QSingleton.hpp"

#include <QObject>
#include <QHash>
#include <QReadWriteLock>
#include <QVariant>

class   QDomDocument;
class   QDomElement;
class   QString;

/**
 * \struct SettingsPart
 * \brief Represent a part of the Settings.
 *
 *      This is used to group different settings, and to be able to easily
 *      save or load just a part of all the settings used by the application.
 */

struct  SettingsPart
{
    typedef QHash<QString, SettingValue*>    ConfigPair;

    SettingsPart() {}

    /**
     * \brief the HashList containing the settings
     */

    ConfigPair                  m_data;

    /**
     * \brief The ReadWriteLock used for when we need to read / write the settingsPart.
     */

    QReadWriteLock              m_lock;

    /**
     * \brief   This flag is used when the SettingsPart is
     *          a readOnly Part.
     */

    bool                        m_rdOnly;

    private:
        SettingsPart( const SettingsPart& );
        SettingsPart&   operator =( const SettingsPart& );
};

/**
 * \class SettingsManager
 * \brief Will manage everything related to the settings.
 */

class   SettingsManager : public QObject, public QSingleton<SettingsManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( SettingsManager )

    friend class QSingleton<SettingsManager>;
    public:

        /**
         * \brief Set a value to an existing or a new SettingValue
         *
         * If the SettingsPart part does not exist, it will be created
         * \sa      commit()
         * \param   part    the part in wich the value will be set
         * \param   key     the key of the setting
         * \param   value   the value of the setting
         */

        void                setValue( const QString& part, const QString& key, const QVariant& value );

        /**
         * \brief   get a settings value for a given name
         *
         * if the settings is not in part, return the default value
         * for this setting
         * \param   part    the part where the settings is stored
         * \param   key     the setting's name
         * \return  the settingPart named part
         * \warning if part does not exist, return a NULL pointer
         */

        const SettingValue* getValue( const QString& part, const QString& key ) const;

        /**
         * \brief getter for a settingsPart
         * \param   part    the name of the part we wish to get
         * \return  The settingsPart named part
         * \warning returns a NULL pointer if the SettingsPart part does not exist
         */

        const SettingsPart* getConfigPart( const QString& part ) const;

        /**
         * \brief save a settingPart into a DomDocument
         *
         * This will only save one part, to save all the preferences, you have to call
         * this method with all the name of the existing SettingsPart
         * \param   part    The part to save
         * \param   xmlfile The QDomDocument used to create new QDomElements
         * \param   root    The root Element of the xml file in wich the project will be saved.
         */

        void                saveSettings( const QString& part, QDomDocument& xmlfile, QDomElement& root );

        /**
          * \brief  load a Settings part from a QDomElement
          *
          * after all settings contained in the donElement have been loaded, this method will emit the
          * settingsLoaded() signal
          * \param  part        the Settings part to Load. The settings part must exist in order to be loaded
          * \param  settings    the QDomElement containing the settingPart to load.
          */

        void                loadSettings( const QString& part, const QDomElement& settings );

        /**
         * \brief   add a new SettingsPart
         * \param   name    the name of the SettingPart that need to be created
         *
         * The SettingsPart is not created if name is already used by another
         * SettingsPart
         */

        void                addNewSettingsPart( const QString& name );

        /**
         * \brief commit all change made into the settingManager
         *
         * This will commit all the settings modified with the SetValue method
         * that have been stored in a temporary Hashlist to the settingManager HashList.
         * It allow us to discard all the modified settings that have been modified. like when the user
         * press the cancel button wile configuring the project settings via the project wizard
         * \sa flush()
         */

        void                commit();

        /**
         * \brief clean the temporary value stored in the SettingsManager
         *
         * this method allow the user to cancel all the modifications made to the settingsManager
         * between two calls of the commit method.
         * this method is also called at the end of the commit method
         */

        void                flush();

        /**
         * \brief Will load the default value of the application and the project settings
         *
         * the defaults are only loaded once
         */

        static void         loadDefaultsSettings();

        /**
         * \brief Return the only available instance of the settingsManager
         */

        static SettingsManager* getInstance();

    private:
        SettingsManager( QObject* parent = 0 );
        ~SettingsManager();

        /**
         * \brief the HashList containing the SettingsPart available
         */

        QHash<QString, SettingsPart*>           m_data;

        /**
         * \brief the temporary HashList containing the SettingsPart that have been modified between two commits
         */

        QHash<QString, SettingsPart*>           m_tempData;

        /**
         * \brief The ReadWriteLock used for when we need to read / write the settingsPart haslist.
         */

        mutable QReadWriteLock                  m_globalLock;

        /**
         * \brief this boolean is used to check if the defaults dettings have already been loaded
         */
        static bool                             m_defaultLoaded;

    signals:

        /**
         * \brief This signal is emmited when the settings have been loaded from a domDocument
         */

        void                settingsLoaded();
};

#endif

