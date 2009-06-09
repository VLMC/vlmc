/*****************************************************************************
 * ModuleManager.cpp: Manage modules loading
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

#include <QtDebug>

#include <QDir>
#include <QStringList>

#include "ModuleManager.h"

ModuleManager::ModuleManager()
{
}

bool        ModuleManager::loadModules()
{
    QFileInfo   moduleDir( "modules/" );
    return loadSubDir( moduleDir );
}

bool        ModuleManager::loadSubDir( const QFileInfo& dir )
{
    if ( dir.exists() == false || dir.isDir() == false )
        return false;
    QDir        subDir( dir.absoluteFilePath() );

    if ( subDir.exists() == false )
    {
        qDebug() << "Modules directory was not found. This is usually a bad thing.";
        return false;
    }
    subDir.setFilter( QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot );
    QStringList nameFilters;
    nameFilters << "*.so" << "*.dll" << "*.dynlib";
    subDir.setNameFilters( nameFilters );

    QFileInfoList results = subDir.entryInfoList();
    if ( results.count() == 0 )
        return true;

    QFileInfoList::const_iterator     it = results.begin();
    QFileInfoList::const_iterator     end = results.end();

    while ( it != end )
    {
        if ( it->isDir() == true )
        {
            loadSubDir( *it );
        }
        else
            qDebug() << it->fileName();
        ++it;
    }
    return true;
}
