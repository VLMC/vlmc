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
#include "SettingsManager.h"

#include <QFile>

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QSettings>
#include <QKeySequence>

#ifndef VLMC_VERSION
#define VLMC_VERSION Unknown
#endif

#define EXPAND( x ) #x
#define STRINGIFY( x ) EXPAND( x )

#define ADD_SHORTCUT(NAME, KEYS)    \
    key = QString("keyboard/").append( QObject::tr( NAME ) ); \
    settMan->setImmediateValue( key, QVariant( QObject::tr( KEYS ) ), \
                                SettingsManager::Vlmc )
static void initShorcuts()
{
    QString key;
    SettingsManager*    settMan = SettingsManager::getInstance();

    ADD_SHORTCUT( "Default mode", "n" );
    ADD_SHORTCUT( "Cut mode", "x" );
    ADD_SHORTCUT( "Launch media preview", "Ctrl+Return" );
    ADD_SHORTCUT( "Start render preview", "Space" );
    //A bit nasty, but we better use what Qt's providing as default shortcut
    ADD_SHORTCUT( "Undo", QKeySequence( QKeySequence::Undo ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Redo", QKeySequence( QKeySequence::Redo ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Help", QKeySequence( QKeySequence::HelpContents ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Quit", "Ctrl+Q" );
    ADD_SHORTCUT( "Preferences", "Alt+P" );
    ADD_SHORTCUT( "Fullscreen", "F" );
    ADD_SHORTCUT( "New project", QKeySequence( QKeySequence::New ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Open project", QKeySequence( QKeySequence::Open ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Save", QKeySequence( QKeySequence::Save ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Save as", "Ctrl+Shift+S" );
    ADD_SHORTCUT( "Close project", QKeySequence( QKeySequence::Close ).toString().toLocal8Bit() );
    ADD_SHORTCUT( "Import media", "Ctrl+I" );
    ADD_SHORTCUT( "Render project", "Ctrl+R" );
    return ;
}

#undef ADD_SHORTCUT

/**
 *  VLMC Entry point
 *  \brief this is the VLMC entry point
 *  \param argc
 *  \param argv
 *  \return Return value of vlmc
 */
int
VLMCmain( int argc, char **argv )
{
    QApplication app( argc, argv );
    app.setApplicationName( "vlmc" );
    app.setOrganizationName( "vlmc" );
    app.setOrganizationDomain( "vlmc.org" );
    app.setApplicationVersion( STRINGIFY(VLMC_VERSION) );
    //QSettings::setDefaultFormat( QSettings::IniFormat );
    //Preferences::changeLang( QSettings().value( "Lang" ).toString() );

#ifdef Q_OS_WIN

    QFile  css(":/styles/windows");
    if ( css.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        QString styleSheet = css.readAll();
        if ( styleSheet != "" )
            app.setStyleSheet( styleSheet );
    }
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

    initShorcuts();

    MainWindow w;
    QSettings   s;
    s.setValue( "VlmcVersion", STRINGIFY( VLMC_VERSION ) );
    w.show();
    return app.exec();
}
