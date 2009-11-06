/*****************************************************************************
 * ProjectPreferences.cpp: VLMC project preferences class
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

//DEBUG
#include <QtDebug>
//!DEBUG

#include "ui_ProjectPreferences.h"
#include "ProjectPreferences.h"
#include "PreferenceWidget.h"
#include "SettingsManager.h"


  ProjectPreferences::ProjectPreferences( QWidget* parent )
: PreferenceWidget( parent )
{
	m_ui.setupUi( this );
}

ProjectPreferences::~ProjectPreferences() { }

void    ProjectPreferences::load()
{
    SettingsManager* settMan = SettingsManager::getInstance();
    const QString& part = m_defaults ? "default" : m_settName;
    QString  Name = settMan->getValue( part, "ProjectName" ).toString();
    QString  Dir = settMan->getValue( part, "ProjectDirectory" ).toString();

    m_ui.ProjectNameLineEdit->setText( Name );
    m_ui.ProjectDirectoryLineEdit->setText( Dir );
}

void    ProjectPreferences::save()
{
    qDebug() << "save proj" << m_settName;
    SettingsManager* settMan = SettingsManager::getInstance();
    QVariant name( m_ui.ProjectNameLineEdit->text() );
    QVariant dir( m_ui.ProjectDirectoryLineEdit->text() );

    settMan->setValue( m_settName, "ProjectName", name );
    settMan->setValue( m_settName, "ProjectDirectory", dir );
}
