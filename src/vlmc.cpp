/*****************************************************************************
 * vlmc.cpp: VLMC launcher for unix
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

#include <wait.h>
#include <unistd.h>

int VLMCmain( int , char** );

int     main( int argc, char **argv )
{
    while ( true )
    {
        pid_t       pid = fork();
        if ( pid < 0 )
            qFatal("Can't fork to launch VLMC. Exiting.");
        if ( pid == 0 )
            return VLMCmain( argc, argv );
        else
        {
            int     status;

            wait( &status );
            if ( WIFEXITED(status) )
            {
                int ret = WEXITSTATUS( status );
                if ( ret == 2 )
                    continue ;
                else
                    break ;
            }
            else
                qCritical() << "Unhandled crash.";
        }
    }
}
