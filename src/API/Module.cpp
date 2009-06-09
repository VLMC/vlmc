/*****************************************************************************
 * Module.cpp: Represents the vlmc's internal representation of a module
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

#include "Module.h"

Module::Module( const QFileInfo& moduleFile )
{
    qDebug() << "Trying to load module :" << moduleFile.absolutePath() + '/' + moduleFile.baseName();
    m_moduleInstance = new QLibrary( moduleFile.absolutePath() + '/' + moduleFile.baseName() );
}

Module::~Module()
{
    m_moduleInstance->unload();
    delete m_moduleInstance;
}

bool        Module::initialize()
{
    m_entryPoint = reinterpret_cast<vlmc_module_entrypoint_t>( m_moduleInstance->resolve( "vlmc_module_entrypoint" ) );
    if ( m_entryPoint == NULL )
    {
        qDebug() << "Can't find module entry point";
        return false;
    }
    m_entryPoint( NULL );
    return true;
}

bool        Module::isLibrary( const QString& fileName )
{
    return QLibrary::isLibrary( fileName );
}
