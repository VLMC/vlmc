/*****************************************************************************
 * MetaDataWorker.cpp: MetaDataManager
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include "MetaDataManager.h"
#include "MetaDataWorker.h"

#include <QtDebug>
#include <QQueue>

MetaDataManager::MetaDataManager() :
        m_computeInProgress( false ),
        m_mediaPlayer( NULL )
{
    m_computingMutex = new QMutex;
}

MetaDataManager::~MetaDataManager()
{
    delete m_computingMutex;
    if ( m_mediaPlayer )
        delete m_mediaPlayer;
}

void    MetaDataManager::launchComputing( Media *media )
{
    m_computeInProgress = true;
    m_mediaPlayer = new LibVLCpp::MediaPlayer;
    MetaDataWorker* worker = new MetaDataWorker( m_mediaPlayer, media );
    connect( worker, SIGNAL( computed() ),
             this, SLOT( computingCompleted() ),
             Qt::DirectConnection );
    worker->compute();
}

void    MetaDataManager::computingCompleted()
{
    QMutexLocker lock( m_computingMutex );

    m_mediaPlayer->stop();
    delete m_mediaPlayer;
    m_mediaPlayer = NULL;
    m_computeInProgress = false;
    if ( m_mediaToCompute.size() != 0 )
        launchComputing( m_mediaToCompute.dequeue() );
}

void    MetaDataManager::computeMediaMetadata( Media *media )
{
    QMutexLocker lock( m_computingMutex );

    if ( m_computeInProgress == true )
    {
        m_mediaToCompute.enqueue( media );
    }
    else
    {
        launchComputing( media );
    }
}
