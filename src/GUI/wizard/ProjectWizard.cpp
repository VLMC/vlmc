/*****************************************************************************
 * ProjectWizard.cpp: Project wizard
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
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

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include "ProjectManager.h"
#include "ProjectWizard.h"
#include "ProjectPreferences.h"
#include "VideoProjectPreferences.h"
#include "AudioProjectPreferences.h"
#include "SettingsManager.h"
#include "PageFactory.h"
#include "WelcomePage.h"
#include "OpenPage.h"

#include <QtDebug>

ProjectWizard::ProjectWizard( QWidget* parent )
    : QWizard( parent )
{
    // Create Wizard

#ifndef Q_WS_MAC
    setWizardStyle( QWizard::ModernStyle );
#endif

    QPixmap logo = QPixmap( ":/images/images/vlmc.png" )
                   .scaledToHeight( 50, Qt::SmoothTransformation );

    setPixmap( QWizard::LogoPixmap, logo );
    setPixmap( QWizard::WatermarkPixmap, QPixmap( ":/images/wizard_watermark" ) );
    setWindowTitle( tr( "Project wizard" ) );

    // Show and connect the help button
    setOption( HaveHelpButton, true );
    connect( this, SIGNAL( helpRequested() ), this, SLOT( showHelp() ) );

    // Create pages

    QWizardPage* welcomePage = new WelcomePage( this );
    QWizardPage* generalPage = PageFactory::generateWizardPage<ProjectPreferences>( "General Settings", this );
    QWizardPage* videoPage = PageFactory::generateWizardPage<VideoProjectPreferences>( "Video Settings", this );
    QWizardPage* audioPage = PageFactory::generateWizardPage<AudioProjectPreferences>( "Audio Settings", this );
    QWizardPage* openPage = new OpenPage( this );

    setPage( Page_Welcome, welcomePage );
    setPage( Page_General, generalPage );
    setPage( Page_Video, videoPage );
    setPage( Page_Audio, audioPage );
    setPage( Page_Open, openPage );

    // Set the start page
    setStartId( Page_Welcome );
}

ProjectWizard::~ProjectWizard()
{
}

void    ProjectWizard::showHelp()
{
    QString message;

    switch ( currentId() )
    {
    case Page_Welcome:
        message = tr( "Choose the appropriate action then click Next to continue." );
        break;
    default:
        message = tr( "This help is likely not to be of any help." );
    }

    QMessageBox::information( this, tr( "Project wizard help" ), message );
}

void    ProjectWizard::accept()
{
    if ( currentId() == Page_Audio )
    {
        SettingsManager::getInstance()->commit();
    }
    if ( WelcomePage::projectPath().length() == 0 )
    {
        ProjectManager::getInstance()->newProject(
            SettingsManager::getInstance()->getValue( "project", "ProjectName" )->get().toString() );
    }
    emit flush();
    QDialog::accept();
    return ;
}

void    ProjectWizard::reject()
{
    SettingsManager::getInstance()->flush();
    emit flush();
    QDialog::reject();
    return ;
}
