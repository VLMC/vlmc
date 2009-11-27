/*****************************************************************************
 * ProjectWizard.cpp: Project wizard
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

#include "ProjectWizard.h"
#include "ProjectPreferences.h"
#include "VideoProjectPreferences.h"
#include "AudioProjectPreferences.h"
#include "PageFactory.h"
#include "LoadPage.h"

ProjectWizard::ProjectWizard( QWidget* parent )
    : QWizard( parent )
{
    //Create Wizard
    QWizardPage* generalPage = PageFactory::generateWizardPage<ProjectPreferences>( "General Settings", this );
    QWizardPage* videoPage = PageFactory::generateWizardPage<VideoProjectPreferences>( "Video Settings", this );
    QWizardPage* audioPage = PageFactory::generateWizardPage<AudioProjectPreferences>( "Audio Settings", this );
    QWizardPage* loadPage = new LoadPage( this );
    addPage( loadPage );
    addPage( generalPage );
    addPage( videoPage );
    addPage( audioPage );
}

ProjectWizard::~ProjectWizard()
{
}

void    ProjectWizard::projectLoaded()
{
    restart();
    QDialog::accept();
}
