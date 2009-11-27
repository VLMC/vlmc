/*****************************************************************************
 * CustomWizardPage.h: Generic wizard page with embeded preferenceWidget
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

#ifndef CUSTOMWIZARDPAGE_H
#define CUSTOMWIZARDPAGE_H

#include <QWidget>
#include <QWizardPage>
#include "PreferenceWidget.h"

class CustomWizardPage : public QWizardPage
{
    Q_OBJECT
    public:
        CustomWizardPage( const char* title, QWidget* parent = 0 );
        ~CustomWizardPage();
        void    setInternalWidget( PreferenceWidget* widg );
        bool    validatePage();
    private:
        PreferenceWidget*   m_widget;

    public slots:
        void        restart();

};

#endif
