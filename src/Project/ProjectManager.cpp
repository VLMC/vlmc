/*****************************************************************************
 * ProjectManager.cpp: Manager the project loading and saving.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#include "ProjectManager.h"
#include "Library.h"
#include "MainWorkflow.h"

ProjectManager::ProjectManager( const QString& filePath )
{
    m_projectFile = new QFile( filePath );
}

ProjectManager::~ProjectManager()
{
    delete m_projectFile;
}

void    ProjectManager::loadTimeline()
{
    QDomElement     root = m_domDocument->documentElement();
    MainWorkflow::getInstance()->loadProject( root.firstChildElement( "timeline" ) );
    deleteLater();
}

void    ProjectManager::loadProject()
{
    m_domDocument = new QDomDocument;
    m_projectFile->open( QFile::ReadOnly );
    m_domDocument->setContent( m_projectFile );
    m_projectFile->close();

    QDomElement     root = m_domDocument->documentElement();

    connect( Library::getInstance(), SIGNAL( projectLoaded() ), this, SLOT( loadTimeline() ) );
    Library::getInstance()->loadProject( root.firstChildElement( "medias" ) );
}

void    ProjectManager::saveProject()
{
    QDomImplementation    implem = QDomDocument().implementation();
    //FIXME: Think about naming the project...
    QString name = "VLMCProject";
    QString publicId = "-//XADECK//DTD Stone 1.0 //EN";
    QString systemId = "http://www-imagis.imag.fr/DTD/stone1.dtd";
    QDomDocument doc(implem.createDocumentType( name, publicId, systemId ) );

    QDomElement rootNode = doc.createElement( "vlmc" );

    Library::getInstance()->saveProject( doc, rootNode );
    MainWorkflow::getInstance()->saveProject( doc, rootNode );

    doc.appendChild( rootNode );

    m_projectFile->open( QFile::WriteOnly );
    m_projectFile->write( doc.toString().toAscii() );
    m_projectFile->close();
    deleteLater();
}
