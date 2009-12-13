/*****************************************************************************
 * OpenPage.cpp: Wizard project openning page
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#include <QDebug>
#include "OpenPage.h"
#include "ProjectManager.h"
#include "ProjectWizard.h"
#include "WelcomePage.h"

OpenPage::OpenPage( QWidget *parent ) :
    QWizardPage(parent)
{
    ui.setupUi( this );

    setTitle( tr( "Project wizard" ) );
    setSubTitle( tr( "Ready to load this project" ) );

    setFinalPage( true );
}

void OpenPage::changeEvent( QEvent *e )
{
    QWizardPage::changeEvent( e );
    switch ( e->type() ) {
    case QEvent::LanguageChange:
        ui.retranslateUi( this );
        break;
    default:
        break;
    }
}

bool OpenPage::validatePage()
{
    ProjectManager* pm = ProjectManager::getInstance();

    if ( !WelcomePage::projectPath().isEmpty() )
        pm->loadProject( WelcomePage::projectPath() );

    return true;
}

int OpenPage::nextId() const
{
    // This an ending page
    return -1;
}
