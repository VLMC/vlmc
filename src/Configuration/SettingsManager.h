/*****************************************************************************
 * SettingsManager.h: Backend settings manager
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
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

#include <Singleton.hpp>

#include <QString>
#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QVariant>

class SettingValue;
class QXmlStreamWriter;
class QDomElement;
class SettingValue;

class   SettingsManager : public QObject, public Singleton<SettingsManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( SettingsManager )
    public:
        typedef QHash<QString, SettingValue*>    SettingHash;
        enum Type
        {
            XML,
            QSett,
            All
        };

        void                        setValue( const QString &key,
                                                    const QVariant &value,
                                                    SettingsManager::Type type = QSett);
        void                        setImmediateValue( const QString &key,
                                         const QVariant &value,
                                         SettingsManager::Type = QSett);
        QVariant                    value( const QString &key,
                                           const QVariant &defaultValue = QVariant(),
                                           SettingsManager::Type type = QSett );
        QHash<QString, QVariant>    group( const QString &groupName,
                                           SettingsManager::Type type = QSett );

        bool                        watchValue( const QString &key,
                                                QObject* receiver,
                                                const char *method,
                                                SettingsManager::Type type );
        void          save() const;
        void          save( QXmlStreamWriter &stream ) const;
        bool          load( QDomElement &element );

        bool          commit( SettingsManager::Type type );
        void          flush();

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
