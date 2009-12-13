/*****************************************************************************
 * SettingValue.h: A setting value that can broadcast its changes
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#ifndef SETTINGVALUE_H
#define SETTINGVALUE_H

#include <QObject>
#include <QVariant>

class   SettingValue : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( SettingValue );
    public:
        SettingValue( const QVariant& val );
        void            set( const QVariant& val );
        const QVariant& get() const;
        QVariant&       get();
    private:
        QVariant        m_val;
    signals:
        void        changed( const QVariant& );
};

#endif // SETTINGVALUE_H
