/*****************************************************************************
 * CrashHandler.cpp: Display the backtrace and some other information when a
 *                      crash occurs
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

#include <signal.h>

#include "BacktraceGenerator.h"
#include "CrashHandler.h"
#include "ui_CrashHandler.h"

CrashHandler::CrashHandler( int sig, QWidget *parent ) :
        QDialog( parent ),
        ui( new Ui::CrashHandler )
{
    ui->setupUi( this );
    connect( ui->okButton, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( ui->restartButton, SIGNAL( clicked() ), this, SLOT( restart() ) );

    QStringList     backtrace = Tools::generateBacktrace( 4 );
    foreach ( QString symb, backtrace )
    {
        ui->backtraceDisplay->insertPlainText( symb );
        ui->backtraceDisplay->insertPlainText( "\n" );
    }
    QString sigName = tr( "Unknown signal" );
    if ( sig == SIGSEGV )
        sigName = "SIGSEGV (Segmentation Fault)";
    else if ( sig == SIGFPE )
        sigName = "SIGFPE (Floating Exception)";
    else if ( sig == SIGABRT )
        sigName = "SIGABRT (Aborted)";
    else if ( sig == SIGILL )
        sigName = "SIGILL (Illegal Instruction)";
    else
        sigName = "Unknown";
    ui->crashDescriptionLabel->setText( tr("A crash occured. Signal received: ") + sigName );
}

CrashHandler::~CrashHandler()
{
    delete ui;
}

void    CrashHandler::changeEvent(QEvent *e)
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

void    CrashHandler::close()
{
    done(1);
}

void    CrashHandler::restart()
{
    done(2);
}
