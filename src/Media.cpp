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
    m_mrl( mrl )
{
    if ( instance == NULL )
    {
        char const *vlc_argv[] =
        {
            "-verbose", "3",
            "--no-skip-frames",
            "--no-audio",
            //"--plugin-path", VLC_TREE "/modules",
            //"--ignore-config", //Don't use VLC's config files
        };
        int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );
        instance = new LibVLCpp::Instance( vlc_argc, vlc_argv );
    }
    m_instance = instance;

    m_vlcMedia = new LibVLCpp::Media( m_instance, mrl );
}

Media::~Media()
{
    if ( m_instance )
        delete m_instance;
    if ( m_vlcMedia )
        delete m_vlcMedia;
    if ( m_vlcMediaPlayer )
        delete m_vlcMediaPlayer;
}

void        Media::loadMedia( const QString& mrl )
{
    if ( m_vlcMedia )
        delete m_vlcMedia;
    m_mrl = mrl;

    setupMedia();
}

void        Media::setupMedia()
{
    if ( m_vlcMediaPlayer )
        delete m_vlcMediaPlayer;

    //Flushing the args into the media :
    QString     param;
    foreach ( param, m_parameters )
        m_vlcMedia->addOption( param.toStdString().c_str() );

    m_vlcMediaPlayer = new LibVLCpp::MediaPlayer( m_vlcMedia );
    qDebug() << "MediaPlayer is not build on top of" << (void*)m_vlcMedia;
}

void        Media::play()
{
    if ( m_vlcMediaPlayer == NULL )
        setupMedia();
    m_vlcMediaPlayer->play();
}

void        Media::addParam( const QString& param )
{
    m_parameters.append( param );
}
