/*****************************************************************************
 * Media.cpp: Generic class for media handling
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
#include "Media.h"

Media::Media(LibVLCpp::Instance* instance, const QString& mrl )
    : m_instance( NULL ), m_vlcMedia( NULL ), m_vlcMediaPlayer( NULL ),
    m_mrl( mrl ), m_snapshot( NULL ), m_instanceOwned( false )
{
    if ( !instance )
    {
        char const *vlc_argv[] =
        {
            //"-vvvvv",
            "--no-skip-frames",
            "--no-audio",
            //"--plugin-path", VLC_TREE "/modules",
            //"--ignore-config", //Don't use VLC's config files
        };
        int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );
        instance = LibVLCpp::Instance::getNewInstance( vlc_argc, vlc_argv );
        m_instanceOwned = true;
    }
    m_instance = instance;

    m_vlcMedia = new LibVLCpp::Media( m_instance, mrl );
}

Media::~Media()
{
    if ( m_instance  && m_instanceOwned == true )
    {
        delete m_instance;
    }
    if ( m_vlcMedia )
    {
        delete m_vlcMedia;
    }
    if ( m_vlcMediaPlayer )
    {
        delete m_vlcMediaPlayer;
    }
}

void        Media::loadMedia( const QString& mrl )
{
    if ( m_vlcMedia )
        delete m_vlcMedia;
    m_mrl = mrl;

    m_vlcMedia = new LibVLCpp::Media( m_instance, mrl );
}

void        Media::setupMedia()
{
//    if ( m_vlcMediaPlayer )
//        delete m_vlcMediaPlayer;

    //Flushing the args into the media :
    QString     param;
    foreach ( param, m_parameters )
        m_vlcMedia->addOption( param.toStdString().c_str() );

   // m_vlcMediaPlayer = new LibVLCpp::MediaPlayer( m_vlcMedia );
}

void        Media::play()
{
    if ( !m_vlcMediaPlayer )
        setupMedia();
    m_vlcMediaPlayer->play();
}

void        Media::pause()
{
    if ( m_vlcMediaPlayer == NULL )
        return;
    m_vlcMediaPlayer->pause();
}

void        Media::stop()
{
    if ( m_vlcMediaPlayer == NULL )
        return;
    m_vlcMediaPlayer->stop();
}

void        Media::addParam( const QString& param )
{
    m_parameters.append( param );
}

void        Media::setDrawable( WId handle )
{
    m_vlcMediaPlayer->setDrawable( handle );
}

qint64      Media::getLength()
{
    return m_vlcMediaPlayer->getLength();
}

qint64      Media::getTime()
{
    return m_vlcMediaPlayer->getTime();
}

void        Media::setTime( qint64 time )
{
    m_vlcMediaPlayer->setTime( time );
}

bool        Media::isPlaying()
{
    return m_vlcMediaPlayer->isPlaying();
}

float       Media::getPosition()
{
    return m_vlcMediaPlayer->getPosition();
}

void        Media::setPosition( float pos )
{
    m_vlcMediaPlayer->setPosition( pos );
}

void        Media::setSnapshot( QPixmap* snapshot )
{
    //TODO: check for mem leaks.
    m_snapshot = snapshot;
}

const QPixmap&    Media::getSnapshot() const
{
    if ( m_snapshot )
        return *m_snapshot;
    //TODO: instanciate this as a static pixmap
    return QPixmap( ":/images/images/vlmc.png" );
}
