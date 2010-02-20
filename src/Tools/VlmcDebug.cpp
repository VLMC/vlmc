/*****************************************************************************
 * VlmcDebug.cpp: Debug tools for VLMC
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

#include <QCoreApplication>
#include <QStringList>

#include "SettingsManager.h"
#include "VlmcDebug.h"

VlmcDebug::VlmcDebug() : m_logFile( NULL )
{
    //setup log level :
    QStringList args = qApp->arguments();
    if ( args.indexOf( QRegExp( "-vvv*" ) ) >= 0 )
        SettingsManager::getInstance()->setImmediateValue( "private/LogLevel", QtDebugMsg, SettingsManager::Vlmc );
    else if ( args.contains( "-v" ) == true )
        SettingsManager::getInstance()->setImmediateValue( "private/LogLevel", QtWarningMsg, SettingsManager::Vlmc );
    else
        SettingsManager::getInstance()->setImmediateValue( "private/LogLevel", QtCriticalMsg, SettingsManager::Vlmc );

    int pos = args.indexOf( QRegExp( "--logfile=.*" ) );
    if ( pos > 0 )
    {
        QString arg = args[pos];
        QString logFile = arg.mid( 10 );
        if ( logFile.length() <= 0 )
            qWarning() << tr("Invalid value supplied for argument --logfile" );
        else
            SettingsManager::getInstance()->setImmediateValue( "private/LogFile", logFile, SettingsManager::Vlmc );
    }


//    QVariant setVal = SettingsManager::getInstance()->value( "private/LogFile", "log.vlmc", SettingsManager::Vlmc );
//    SettingsManager::getInstance()->watchValue( "private/LogFile", this,
//                                              SLOT( logFileChanged( const QVariant& ) ),
//                                              SettingsManager::Vlmc );
//    QObject::connect( qApp,
//                      SIGNAL( aboutToQuit() ),
//                      this,
//                      SLOT( deleteLater() ) );
//    QString logFile = setVal.toString();
//    if ( logFile.isEmpty() == false )
//    {
//        m_logFile = new QFile( logFile );
//        m_logFile->open( QFile::WriteOnly | QFile::Truncate );
//    }
}

VlmcDebug::~VlmcDebug()
{
    delete m_logFile;
}

void    VlmcDebug::setup()
{
//    qInstallMsgHandler( VlmcDebug::vlmcMessageHandler );
}

void    VlmcDebug::logFileChanged( const QVariant& logFileV )
{
    QString logFile = logFileV.toString();
    if ( logFile.isEmpty() == true )
    {
        m_logFile->close();
        return ;
    }
    if ( logFile == m_logFile->fileName() )
    {
        fprintf( stderr, "Trying to use the same log file. Nothing will be done" );
        return ;
    }
    m_logFile->close();
    m_logFile->setFileName( logFile );
    m_logFile->open( QFile::Append | QFile::Truncate );
}

void    VlmcDebug::vlmcMessageHandler( QtMsgType type, const char* msg )
{
    if ( VlmcDebug::getInstance()->m_logFile != NULL )
    {
        VlmcDebug::getInstance()->m_logFile->write( msg );
        VlmcDebug::getInstance()->m_logFile->write( "\n" );
    }
//    if ( type != QtFatalMsg
//         && type < SettingsManager::getInstance()->value( "private/LogLevel", QtDebugMsg, SettingsManager::Vlmc ).toInt() )
//        return ;
    switch ( type )
    {
    case QtDebugMsg:
        fprintf(stderr, "%s\n", msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s\n", msg);
        abort();
    }
}
