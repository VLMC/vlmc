/*****************************************************************************
 * SettingValue.h: A setting value that can broadcast its changes
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

/**
 * 'class SettingValue
 *
 * \brief represent a setting value
 *
 */
class   SettingValue : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( SettingValue );
    public:
        enum    Type
        {
            Int,
            Double,
            String,
            Bool,
            Language,
            KeyboardShortcut
        };
        /**
         *  \brief      Constructs a setting value with its default value and description
         *
         *  \param      defaultValue    The setting default value.
         *  \param      desc            The setting description
         */
        SettingValue( Type type, const QVariant& defaultValue, const QString& name,
                      const QString& desc );

        /**
         * \brief setter for the m_val member
         * \param   val the value wich will be affected to m_val
         */

        void            set( const QVariant& val );

        /**
         * \brief getter for the m_val member
         */
        const QVariant& get() const;
        /**
         *  \return The setting's description
         */
        const QString   &description() const;
        /**
         *   \brief     Set the setting to its default value.
         */
        void            restoreDefault();

        const QString   &name() const;
        Type            type() const;

    private:
        /**
         * \brief the QVariant containingthe value of the settings
         */
        QVariant        m_val;
        QVariant        m_defaultVal;
        QString         m_name;
        QString         m_desc;
        Type            m_type;
    signals:
        /**
         * \brief This signal is emmited while the m_val
         *        member have been modified
         */
        void        changed( const QVariant& );
};

#endif // SETTINGVALUE_H
