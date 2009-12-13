/*****************************************************************************
 * CrashHandler.cpp: Display the backtrace and some other information when a
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

#include "CrashHandler.h"
#include "ui_CrashHandler.h"

#include <execinfo.h>
#include <signal.h>


CrashHandler::CrashHandler( int sig, QWidget *parent ) :
        QDialog( parent ),
        ui( new Ui::CrashHandler )
{
    ui->setupUi( this );

    void    *buff[CrashHandler::backtraceSize];
    int     nbSymb = backtrace( buff, CrashHandler::backtraceSize );
    char**  backtraceStr = backtrace_symbols( buff, nbSymb );
    for ( int i = 0; i < nbSymb; ++i )
        ui->backtraceDisplay->insertPlainText( QString( backtraceStr[i]) + "\n" );
    free(backtraceStr);

    QString sigName = tr( "Unknown signal" );
    if ( sig == SIGSEGV )
        sigName = "SIGSEGV (Segmentation Fault)";
    if ( sig == SIGFPE )
        sigName = "SIGFPE (Floating Exception)";
    else
        sigName = "Unknown";
    ui->crashDescriptionLabel->setText( tr("A crash occured. Signal received: ") + sigName );
}

CrashHandler::~CrashHandler()
{
    delete ui;
}

void CrashHandler::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        ui->retranslateUi( this );
        break;
    default:
        break;
    }
}
