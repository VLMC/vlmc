/*****************************************************************************
 * Preferences.h: Preferences window
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
 *          Clement CHAVANCE <kinder@vlmc.org>
 *          Hugo Beauzee-Luyssen <beauze.h@gmail.com>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "PreferenceWidget.h"
#include "SettingsManager.h"

#include "ui_LanguagePreferences.h"
#include <QTranslator>
#include <QtDebug>

#define TS_PREFIX "vlmc_"

class LanguagePreferences : public PreferenceWidget
{
    Q_OBJECT
    public:
        LanguagePreferences( QWidget* parent = 0 );
        ~LanguagePreferences();
        void    load();
        void    save();
        void    changeLang(QString lang);

    protected:
        virtual void changeEvent( QEvent *e );

    private:
        Ui::LanguagePreferences m_ui;
        static QTranslator*     m_currentLang;
        SettingsManager::Type   m_type;
};

#endif // PREFERENCES_H
