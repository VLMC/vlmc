/*****************************************************************************
 * ClipRenderer.cpp: Render from a Clip (mainly for previewing purpose)
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarrière <geoffroylaca@gmail.com>
 *          Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#include "ClipRenderer.h"

ClipRenderer::ClipRenderer() :
    GenericRenderer(),
    m_clipLoaded( false ),
    m_vlcMedia( NULL ),
    m_selectedMedia( NULL ),
    m_mediaChanged( false )
{
    connect( m_mediaPlayer,     SIGNAL( stopped() ),            this,   SLOT( __videoStopped() ) );
    connect( m_mediaPlayer,     SIGNAL( paused() ),             this,   SLOT( __videoPaused() ) );
    connect( m_mediaPlayer,     SIGNAL( playing() ),            this,   SLOT( __videoPlaying() ) );
    connect( m_mediaPlayer,     SIGNAL( positionChanged() ),    this,   SLOT( __positionChanged() ) );
    connect( m_mediaPlayer,     SIGNAL( endReached() ),         this,   SLOT( __endReached() ) );
}

ClipRenderer::~ClipRenderer()
{
    stop();
}

void        ClipRenderer::setMedia( const Media* media )
{
    m_selectedMedia = media;
    if ( m_isRendering == true )
        m_mediaChanged = true;
    else
        m_clipLoaded = false;
}

void        ClipRenderer::startPreview()
{
    if ( m_selectedMedia == NULL )
        return ;
    //If an old media is found, we delete it, and disconnect
    if ( m_vlcMedia != NULL )
        delete m_vlcMedia;
    m_vlcMedia = new LibVLCpp::Media( m_selectedMedia->getFileInfo()->absoluteFilePath() );

    m_mediaPlayer->setMedia( m_vlcMedia );

    m_mediaPlayer->play();
    m_clipLoaded = true;
    m_isRendering = true;
    m_paused = false;
    m_mediaChanged = false;
}

void        ClipRenderer::setPosition( float newPos )
{
    if ( m_clipLoaded == false || m_isRendering == false )
        return ;
    m_mediaPlayer->setPosition( newPos );
}

void        ClipRenderer::stop()
{
    if ( m_clipLoaded == true && m_isRendering == true )
    {
        m_isRendering = false;
        m_mediaPlayer->stop();
        m_paused = false;
        if ( m_mediaChanged == true )
            m_clipLoaded = false;
    }
}

void        ClipRenderer::togglePlayPause( bool forcePause )
{
    if ( m_clipLoaded == false )
    {
        startPreview();
        return ;
    }
    //If for some reason, nothing was loaded in startPreview(), we just return.
    if ( m_clipLoaded == false )
        return ;

    if ( m_paused == false && m_isRendering == true )
    {
        m_mediaPlayer->pause();
        m_paused = true;
    }
    else if ( forcePause == false )
    {
        if ( m_isRendering == false )
            m_isRendering = true;
        m_mediaPlayer->play();
        m_paused = false;
    }
}

void        ClipRenderer::nextFrame()
{
    if ( m_isRendering == true && m_paused == true )
    {
        qint64   interval =  static_cast<qint64>( (1.0f / m_mediaPlayer->getFps()) * 1000.0f );
        m_mediaPlayer->setTime( m_mediaPlayer->getTime() + interval );
    }
}

void        ClipRenderer::previousFrame()
{
    if ( m_isRendering == false && m_paused == true )
    {
        qint64   interval =  static_cast<qint64>( (1.0f / m_mediaPlayer->getFps()) * 1000.0f );
        m_mediaPlayer->setTime( m_mediaPlayer->getTime() - interval );
    }
}

void        ClipRenderer::mediaUnloaded( const QUuid& uuid )
{
    if ( m_selectedMedia != NULL && m_selectedMedia->getUuid() == uuid )
    {
        m_mediaPlayer->stop();
        qDebug() << "Media unloaded";
        m_clipLoaded = false;
        m_selectedMedia = NULL;
        m_isRendering = false;
        m_paused = false;
    }
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////
void        ClipRenderer::__videoPaused()
{
    emit paused();
}

void        ClipRenderer::__videoStopped()
{
    emit stopped();
}

void        ClipRenderer::__videoPlaying()
{
    emit playing();
}

void        ClipRenderer::__positionChanged()
{
    if ( m_clipLoaded == false)
        return ;
    emit positionChanged( m_mediaPlayer->getPosition() );
}

void        ClipRenderer::__endReached()
{
    m_mediaPlayer->stop();
    m_isRendering = false;
    emit endReached();
}
