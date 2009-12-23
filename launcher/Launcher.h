/*****************************************************************************
 * Launcher.h: Will launch vlmc and watch for events.
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

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QProcess>
#include <QStringList>

class   Launcher : public QObject
{
    Q_OBJECT

    public:
        static const int    cleanExit = 0;
        static const int    crashExit = 1;
        static const int    crashWithRestart = 2;

        Launcher( int argc, char** argv, QObject* parent = NULL );
        void        start();
    public slots:
        void        stopped( int retCode, QProcess::ExitStatus exitType );
    private:
        QProcess*   m_process;
        QStringList m_argv;
};

#endif // LAUNCHER_H
