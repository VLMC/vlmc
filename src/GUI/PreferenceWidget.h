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

#include <QSettings>
#include <QWidget>

class   PreferenceWidget
{
    public:
        PreferenceWidget( QWidget* widget = 0 );
        virtual ~PreferenceWidget() {}

        virtual bool    load() = 0;
        virtual void    save( QSettings& settings ) = 0;

        QWidget*        widget() const;
        void            setWidget( QWidget* widget );

    private:
        QWidget*        m_widget;
};

#endif
