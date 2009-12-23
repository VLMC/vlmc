/*****************************************************************************
 * Launcher.cpp : Will launch VLMC and watch for events
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
#include <QCoreApplication>

#include "Launcher.h"

Launcher::Launcher( int argc, char** argv, QObject* parent ) : QObject( parent )
{
    m_process = new QProcess;
    connect( m_process, SIGNAL( finished( int, QProcess::ExitStatus ) ),
             this, SLOT( stopped( int, QProcess::ExitStatus ) ) );
    for ( int i = 1; i < argc; ++i )
        m_argv << argv[i];
    m_process->setReadChannelMode( QProcess::ForwardedChannels );
}

void    Launcher::start()
{
    //If you put "vlmc" here, it will probably result in a fork bomb :)
    m_process->start( "bin/vlmc", m_argv );
}

void    Launcher::stopped( int exitCode, QProcess::ExitStatus )
{
    switch ( exitCode )
    {
    case    Launcher::cleanExit:
    case    Launcher::crashExit:
        QCoreApplication::exit( exitCode );
        break ;
    case    Launcher::crashWithRestart:
        m_process->start( "bin/vlmc", m_argv );
        return ;
    }
}
