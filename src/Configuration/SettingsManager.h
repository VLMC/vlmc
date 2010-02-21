/*****************************************************************************
 * SettingsManager.h: Backend settings manager
 *****************************************************************************
 * Copyright (C) 2008-2009 VideoLAN
 *
 * Authors: Clement CHAVANCE <chavance.c@gmail.com>
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
#include "Singleton.hpp"

#include <QString>
#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QVariant>

class SettingValue;
class QXmlStreamWriter;
class QDomElement;
class QDomDocument;


//Var helpers :
#define VLMC_GET_STRING( key )  SettingsManager::getInstance()->value( key )->get().toString()
#define VLMC_GET_INT( key )     SettingsManager::getInstance()->value( key )->get().toInt()
#define VLMC_GET_UINT( key )    SettingsManager::getInstance()->value( key )->get().toUInt()
#define VLMC_GET_DOUBLE( key )  SettingsManager::getInstance()->value( key )->get().toDouble()
#define VLMC_GET_BOOL( key )    SettingsManager::getInstance()->value( key )->get().toBool()

#define VLMC_PROJECT_GET_STRING( key )  SettingsManager::getInstance()->value( key, SettingsManager::Project )->get().toString()
#define VLMC_PROJECT_GET_INT( key )     SettingsManager::getInstance()->value( key, SettingsManager::Project )->get().toInt()
#define VLMC_PROJECT_GET_UINT( key )    SettingsManager::getInstance()->value( key, SettingsManager::Project )->get().toUInt()
#define VLMC_PROJECT_GET_DOUBLE( key )  SettingsManager::getInstance()->value( key, SettingsManager::Project )->get().toDouble()
#define VLMC_PROJECT_GET_BOOL( key )    SettingsManager::getInstance()->value( key, SettingsManager::Project )->get().toBool()


#define VLMC_CREATE_PROJECT_VAR( type, key, defaultValue, name, desc )  \
SettingsManager::getInstance()->createVar( type, key, defaultValue, QObject::tr( name ), \
                                           QObject::tr(desc), SettingsManager::Project );

#define VLMC_CREATE_PROJECT_INT( key, defaultValue, name, desc )  \
        VLMC_CREATE_PROJECT_VAR( SettingValue::Int, key, defaultValue, name, desc )
#define VLMC_CREATE_PROJECT_STRING( key, defaultValue, name, desc )  \
        VLMC_CREATE_PROJECT_VAR( SettingValue::String, key, defaultValue, name, desc )
#define VLMC_CREATE_PROJECT_DOUBLE( key, defaultValue, name, desc )  \
        VLMC_CREATE_PROJECT_VAR( SettingValue::Double, key, defaultValue, name, desc )
#define VLMC_CREATE_PROJECT_BOOL( key, defaultValue, name, desc )  \
        VLMC_CREATE_PROJECT_VAR( SettingValue::Bool, key, defaultValue, name, desc )

#define VLMC_CREATE_PREFERENCE( type, key, defaultValue, name, desc )  \
SettingsManager::getInstance()->createVar( type, key, defaultValue, QObject::tr( name ),  \
                                           QObject::tr(desc), SettingsManager::Vlmc );

#define VLMC_CREATE_PREFERENCE_INT( key, defaultValue, name, desc )  \
        VLMC_CREATE_PREFERENCE( SettingValue::Int, key, defaultValue, name, desc )
#define VLMC_CREATE_PREFERENCE_STRING( key, defaultValue, name, desc )  \
        VLMC_CREATE_PREFERENCE( SettingValue::String, key, defaultValue, name, desc )
#define VLMC_CREATE_PREFERENCE_DOUBLE( key, defaultValue, name, desc )  \
        VLMC_CREATE_PREFERENCE( SettingValue::Double, key, defaultValue, name, desc )
#define VLMC_CREATE_PREFERENCE_LANGUAGE( key, defaultValue, name, desc )  \
        VLMC_CREATE_PREFERENCE( SettingValue::Language, key, defaultValue, name, desc )
#define VLMC_CREATE_PREFERENCE_KEYBOARD( key, defaultValue, name, desc )  \
        VLMC_CREATE_PREFERENCE( SettingValue::KeyboardShortcut, key, defaultValue, name, desc )
#define VLMC_CREATE_PREFERENCE_BOOL( key, defaultValue, name, desc )  \
        VLMC_CREATE_PREFERENCE( SettingValue::Bool, key, defaultValue, name, desc )

class   SettingsManager : public QObject, public Singleton<SettingsManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( SettingsManager )
    public:
        typedef QHash<QString, SettingValue*>    SettingHash;
        enum Type
        {
            Project,
            Vlmc
        };

        void                        setValue( const QString &key,
                                                const QVariant &value,
                                                SettingsManager::Type type = Vlmc);
        void                        setImmediateValue( const QString &key,
                                                        const QVariant &value,
                                                        SettingsManager::Type = Vlmc);
        SettingValue                *value( const QString &key,
                                            SettingsManager::Type type = Vlmc );
        SettingHash                 group( const QString &groupName,
                                            SettingsManager::Type type = Vlmc );

        void                        createVar( SettingValue::Type type, const QString &key,
                                               const QVariant &defaultValue,
                                               const QString &name, const QString &desc,
                                               Type varType = Vlmc );
        bool                        watchValue( const QString &key,
                                                QObject* receiver,
                                                const char *method,
                                                SettingsManager::Type type,
                                                Qt::ConnectionType cType = Qt::AutoConnection );
        void                        save() const;
        void                        save( QDomDocument &xmlfile, QDomElement& root ) const;
        bool                        load( const QDomElement &element );

        bool                        commit( SettingsManager::Type type );
        void                        flush();

    private:
        friend class Singleton<SettingsManager>;
        SettingsManager();
        ~SettingsManager();

        SettingHash                 m_classicSettings;
        SettingHash                 m_xmlSettings;

        SettingHash                 m_tmpClassicSettings;
        SettingHash                 m_tmpXmlSettings;

        mutable QReadWriteLock      m_rwLock;
};

#endif
