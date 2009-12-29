/*****************************************************************************
 * vlmc.h : contains the definition for every macro and prototypes used
 *          used program wide.
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

#ifndef VLMC_H
#define VLMC_H

#include <QtGlobal>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#ifdef Q_OS_UNIX
#define SleepMS( x ) usleep( (x) * 1000 )
#else
#define SleepMS( x ) Sleep( x )
#endif

//TODO: THIS HAS TO GO REALLY REALLY REALLY SOON !!!!
//FIXME:
//UGLY:
#define VIDEOWIDTH 640
#define VIDEOHEIGHT 480

enum ToolButtons
{
    TOOL_DEFAULT,
    TOOL_CUT,
};

#endif // VLMC_H
