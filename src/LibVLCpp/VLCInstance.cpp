/*****************************************************************************
 * VLCInstance.cpp: Binding for libvlc instances
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

#include <cassert>
#include "VLCInstance.h"
#include <QtDebug>

using namespace LibVLCpp;

Instance::Instance()
{
    char const *argv[] =
    {
//        "-vvvvv",
        "--no-skip-frames",
//        "--intf", "dummy",
        "--text-renderer", "dummy",
        //"--no-audio",
        //"--plugin-path", VLC_TREE "/modules",
        "--disable-screensaver",
        "--ignore-config", //Don't use VLC's config files
        "--no-overlay",
    };
    int argc = sizeof( argv ) / sizeof( *argv );

    m_internalPtr = libvlc_new( argc, argv, m_ex );
    CheckVlcppException(m_ex);
}

Instance::~Instance()
{
    libvlc_release( m_internalPtr );
}
