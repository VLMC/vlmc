/*****************************************************************************
 * UnixBacktraceGenerator.cpp: Unix backtrace generation
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

#include "BacktraceGenerator.h"

#include <execinfo.h>
#include <cxxabi.h>
#include <stdlib.h> //free()

#include <QStringList>

QStringList     Tools::generateBacktrace( int levelsToSkip )
{
    void    *buff[backtraceSize];
    QStringList res;

    int     nbSymb = backtrace( buff, backtraceSize );
    char**  backtraceStr = backtrace_symbols( buff, nbSymb );
    char*   symbName;
    char*   mangledName;
    int     status;
    int     pos;
    for ( int i = levelsToSkip; i < nbSymb; ++i )
    {
        mangledName = strchr( backtraceStr[i], '(' );
        char* endPos = strchr( mangledName, '+' );
        if ( endPos != NULL && endPos != NULL )
        {
            pos = endPos - mangledName;
            char *copy = strdup( mangledName + 1 );  //Skipping the parenthesis
            copy[pos - 1] = 0;
            symbName = abi::__cxa_demangle( copy, NULL, 0, &status);
            if ( status == 0 )
            {
                res.append( QString( symbName ) );
                free( symbName );
                continue ;
            }
            free(symbName);
            free(copy);
        }
        res.append( backtraceStr[i] );
    }
    free(backtraceStr);
    return res;
}
