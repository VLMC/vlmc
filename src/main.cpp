/*****************************************************************************
 * main.cpp: VLMC main
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

/** \file
 *  This file contain the main function.
 *  It will initialize the QT application,
 *  and start it.
 */

#include "MainWindow.h"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QSettings>

#ifndef VLMC_VERSION
#define VLMC_VERSION Unknown
#endif

#define EXPAND( x ) #x
#define STRINGIFY( x ) EXPAND( x )

/**
 *  VLMC Entry point
 *  \brief this is the VLMC entry point
 *  \param argc
 *  \param argv
 *  \return Return value of vlmc
 */
int
main( int argc, char **argv )
{
    QApplication app( argc, argv );
    app.setApplicationName( "vlmc" );
    app.setOrganizationName( "vlmc" );
    app.setOrganizationDomain( "vlmc.org" );
    app.setApplicationVersion( STRINGIFY(VLMC_VERSION) );
    //QSettings::setDefaultFormat( QSettings::IniFormat );
    //Preferences::changeLang( QSettings().value( "Lang" ).toString() );

#ifdef Q_OS_WIN
    // Ugly workaround
    app.setStyle( "plastique" );
#endif

    // Creating the color palette
    QPalette p;
    p.setColor( QPalette::WindowText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::Button,           QColor( 104, 103, 103, 255 ) );
    p.setColor( QPalette::Light,            QColor( 156, 155, 155, 255 ) );
    p.setColor( QPalette::Midlight,         QColor( 130, 129, 129, 255 ) );
    p.setColor( QPalette::Dark,             QColor( 52,  51,  51,  255 ) );
    p.setColor( QPalette::Mid,              QColor( 69,  68,  68,  255 ) );
    p.setColor( QPalette::Text,             QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::BrightText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::ButtonText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::Base,             QColor( 104, 103, 103, 255 ) );
    p.setColor( QPalette::Window,           QColor( 73,  72,  72,  255 ) );
    p.setColor( QPalette::Shadow,           QColor( 0,   0,   0,   255 ) );
    p.setColor( QPalette::AlternateBase,    QColor( 52,  51,  51,  255 ) );
    p.setColor( QPalette::ToolTipBase,      QColor( 255, 255, 220, 255 ) );
    p.setColor( QPalette::ToolTipText,      QColor( 0,   0,   0,   255 ) );
    p.setColor( QPalette::WindowText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::WindowText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::WindowText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::WindowText,       QColor( 255, 255, 255, 255 ) );
    p.setColor( QPalette::Link,             QColor( 177, 202, 0,   255 ) );
    p.setColor( QPalette::LinkVisited,      QColor( 177, 202, 0,   255 ) );
    app.setPalette( p );

    MainWindow w;
    QSettings   s;
    s.setValue( "VlmcVersion", STRINGIFY( VLMC_VERSION ) );
    w.show();
    return app.exec();
}
