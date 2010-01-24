/*****************************************************************************
 * Module.cpp: Represents the vlmc's internal representation of a module
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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

void        Module::initInternalPtr()
{
    m_p_module = new vlmc_module_internal_t;
    m_p_module->public_module.f_ratio = 0;
    m_p_module->public_module.psz_name = NULL;

    m_p_module->public_module.p_callbacks = new vlmc_callback_t;
    m_p_module->public_module.p_callbacks->pf_open = NULL;
    m_p_module->public_module.p_callbacks->pf_close = NULL;
    m_p_module->public_module.p_callbacks->pf_process = NULL;

    m_p_module->public_module.p_output = new vlmc_output_t;
    m_p_module->public_module.p_output->i_height = 0;
    m_p_module->public_module.p_output->i_width = 0;
    m_p_module->public_module.p_output->p_buffer = NULL;
}

bool        Module::initialize()
{
#ifdef Q_WS_WIN
    qDebug() << "Loading of modules is currently disabled under Windows.";
    return false;
#else
    m_entryPoint = reinterpret_cast<vlmc_module_entrypoint_t>( m_moduleInstance->resolve( "vlmc_module_entrypoint" ) );
    if ( m_entryPoint == NULL )
    {
        qDebug() << "Can't find module entry point";
        return false;
    }
    initInternalPtr();
    m_entryPoint( reinterpret_cast<vlmc_module_t*>( m_p_module ) );
    if ( m_p_module->public_module.psz_name == NULL )
    {
        qDebug() << "No name set. Invalid module";
        return false;
    }
    return true;
#endif
}

bool        Module::isLibrary( const QString& fileName )
{
    return QLibrary::isLibrary( fileName );
}
