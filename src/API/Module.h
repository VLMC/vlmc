/*****************************************************************************
 * Module.h: Represents the vlmc's internal representation of a module
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

#ifndef MODULE_H
#define MODULE_H

#include <QLibrary>
#include <QFileInfo>

#include "vlmc_module_internal.h"

class   Module
{
public:
    Module( const QFileInfo& moduleFile );
    ~Module();

    bool                        initialize();

    static bool                 isLibrary( const QString& fileName );

private:
    void                        initInternalPtr();

private:
    QString                     m_name;
    QLibrary*                   m_moduleInstance;
    vlmc_module_entrypoint_t    m_entryPoint;
    vlmc_module_internal_t*     m_p_module;
};

#endif // MODULE_H
