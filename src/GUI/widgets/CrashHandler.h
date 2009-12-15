/*****************************************************************************
 * CrashHandler.h: Display the backtrace and some other information when a
 *                      crash occurs
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

#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QDialog>

namespace Ui
{
    class CrashHandler;
}

class CrashHandler : public QDialog
{
    Q_OBJECT
    public:
        CrashHandler(  int sig, QWidget *parent = 0 );
        ~CrashHandler();

    protected:
        void                    changeEvent(QEvent *e);

    private slots:
        void                    close();
        void                    restart();

    private:
        Ui::CrashHandler*       ui;
        static const int        backtraceSize = 256;
};

#endif // CRASHHANDLER_H
