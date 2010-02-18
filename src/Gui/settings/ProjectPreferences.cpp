/*****************************************************************************
 * ProjectPreferences.cpp: VLMC project preferences class
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

#include "ProjectPreferences.h"

#include "PreferenceWidget.h"
#include "SettingsManager.h"
#include "ProjectManager.h"

#include <QWidget>
//DEBUG
#include <QtDebug>
//!DEBUG


ProjectPreferences::ProjectPreferences( QWidget* parent )
    : PreferenceWidget( parent ),
    m_type( SettingsManager::Project )
{
    m_ui.setupUi( this );
}

ProjectPreferences::~ProjectPreferences() { }

void    ProjectPreferences::load()
{
    SettingsManager* settMan = SettingsManager::getInstance();
    QString  Name = settMan->value( "project/ProjectName",
                                        ProjectManager::unNamedProject,
                                        m_type ).toString();

    m_ui.ProjectNameLineEdit->setText( Name );
}

void    ProjectPreferences::save()
{
    SettingsManager* settMan = SettingsManager::getInstance();
    QVariant name( m_ui.ProjectNameLineEdit->text() );

    settMan->setImmediateValue( "project/ProjectName", name, m_type );
}

void ProjectPreferences::changeEvent( QEvent *e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        break;
    }
}
