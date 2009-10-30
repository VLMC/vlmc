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

#include <QObject>
#include <QVector>
#include <QHash>
#include <QReadWriteLock>
#include <QString>
#include <QVariant>
#include <QDomDocument>

#include "QSingleton.hpp"


class   SettingsManager : public QObject, public QSingleton<SettingsManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( SettingsManager )

    friend class QSingleton<SettingsManager>;
    public:

        void                setValues( QHash<QString, QVariant> );
        void                setValue( const QString& key, QVariant& value );
        const QVariant      getValue( const QString& key ) const;
        void                saveSettings( QDomDocument& xmlfile, QDomElement& root );
        void                loadSettings( const QDomElement& settings );
    private:
        SettingsManager( QObject* parent = 0 );
        ~SettingsManager();

        QHash<QString, QVariant>            m_data;
        mutable QReadWriteLock              m_lock;

    signals:
        void                settingsLoaded();
};

#endif

