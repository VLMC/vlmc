/*****************************************************************************
 * LanguageWidget.h: Handle languge settings
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@vlmc.org>
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

#ifndef LANGUAGEWIDGET_H
#define LANGUAGEWIDGET_H

#include "ISettingsCategorieWidget.h"
#include <stddef.h>

class   SettingValue;
class   QComboBox;
class   QTranslator;

class   LanguageWidget : public ISettingsCategorieWidget
{
    public:
        LanguageWidget( SettingValue *s, QWidget *parent = NULL );
        QWidget*                widget();
        void                    save();

    private:
        SettingValue            *m_setting;
        QComboBox               *m_list;
        QTranslator             *m_translator;
};

#endif // LANGUAGEWIDGET_H
