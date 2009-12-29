/*****************************************************************************
 * PreferenceWidget.h: Abstract class that will be used to save load / preferences
 * values.
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

#ifndef PREFERENCEWIDGET_H
#define PREFERENCEWIDGET_H

#include <QWidget>
#include <QString>

class   PreferenceWidget : public QWidget
{
    Q_OBJECT
    public:
        PreferenceWidget( QWidget* parent = 0 );
        virtual ~PreferenceWidget() {}

        void            setDefaults( bool defaults );
        void            setName( const QString& name );

        virtual void    load() = 0;
        virtual void    save() = 0;

    protected:
        bool            m_defaults;
        QString         m_settName;

    protected slots:
        void            loadThemAll( const QString& part = "default",
                                   bool defaults = false );
};

#endif
