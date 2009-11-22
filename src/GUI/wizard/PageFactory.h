/*****************************************************************************
 * PageFactory.cpp: Factory wich will generate CustomWizardPage
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

#ifndef PAGEFACTORY_H
#define PAGEFACTORY_H

#include <QWidget>
#include "CustomWizardPage.h"

class   PageFactory
{
    public:
        template < typename widgetType >
        static QWizardPage*    generateWizardPage( const char* title, QWidget* parent = 0 )
        {
            CustomWizardPage* page = new CustomWizardPage( title, parent );
            page->setInternalWidget( new widgetType( page ) );

            return page;
        }

    private:
        PageFactory();
        ~PageFactory();
        PageFactory( const PageFactory& );
        PageFactory&    operator = ( const PageFactory& );
};

#endif
