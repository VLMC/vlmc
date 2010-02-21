/*****************************************************************************
 * PreferenceWidget.h: Abstract class that will be used to save load / preferences
 * values.
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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
#include "SettingsManager.h"

class   ISettingsCategorieWidget;
class   SettingValue;

class   PreferenceWidget : public QWidget
{
    Q_OBJECT
    public:
        typedef QList<ISettingsCategorieWidget*>    SettingsList;
        PreferenceWidget( const QString& categorie, SettingsManager::Type type,
                          QWidget* parent = 0 );
        virtual ~PreferenceWidget() {}

        virtual void    load();
        virtual void    save();
    private:
        ISettingsCategorieWidget        *widgetFactory( SettingValue* s );

    private:
        QString         m_categorie;
        SettingsList    m_settings;
};

#endif
