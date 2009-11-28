/*****************************************************************************
 * CustomWizardPage.cpp: Generic wizard page with embeded preferenceWidget
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

#include <QVBoxLayout>
#include <QtDebug>
#include "CustomWizardPage.h"
#include "VideoProjectPreferences.h"

CustomWizardPage::CustomWizardPage( const char* title, QWidget* parent )
    : QWizardPage( parent )
{
    setTitle( tr( title ) );
    if ( parent != 0 )
    {
        QObject::connect( parent,
                          SIGNAL( flush() ),
                          this,
                          SLOT( restart() ) );
    }
}

CustomWizardPage::~CustomWizardPage()
{
    //Nothing to do
}

void    CustomWizardPage::setInternalWidget( PreferenceWidget* widg )
{
    m_widget = widg;
    m_widget->setDefaults( true );
    m_widget->setName( "project" );
    m_widget->load();

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->addWidget( m_widget );
    setLayout( layout );
}


bool    CustomWizardPage::validatePage()
{
    m_widget->save();
    return true;
}

void    CustomWizardPage::restart()
{
    m_widget->load();
}

