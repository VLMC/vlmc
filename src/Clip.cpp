/*****************************************************************************
 * Clip.cpp: Represents a basic container for media informations.
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
#include "Clip.h"

QPixmap*    Clip::defaultSnapshot = NULL;

Clip::Clip( const QString& mrl )
    : m_vlcMedia( NULL ), m_mrl( mrl ), m_snapshot( NULL ), m_begin( 0 ), m_end( -1 ),
    m_length( 0 ), m_width( 0 ), m_height( 0 )
{
    m_vlcMedia = new LibVLCpp::Media( mrl );
    m_uuid = QUuid::createUuid();
    //We avoid creating a fileInfo from the mrl since it can be "fake://" for invmem.
    m_fileInfo = NULL;
}

Clip::Clip( const QFileInfo* fileInfo)
    : m_vlcMedia( NULL ), m_snapshot( NULL ), m_begin( 0 ), m_end( -1 ),
    m_length( 0 ), m_width( 0 ), m_height( 0 )
{
    m_mrl = "file://" + fileInfo->absoluteFilePath();
    m_vlcMedia = new LibVLCpp::Media( m_mrl );
    m_uuid = QUuid::createUuid();
    m_fileInfo = new QFileInfo( *fileInfo );
}

Clip::~Clip()
{
    if ( m_vlcMedia )
    {
        delete m_vlcMedia;
    }
}

void        Clip::loadMedia( const QString& mrl )
{
    if ( m_vlcMedia )
        delete m_vlcMedia;
    m_mrl = mrl;

    m_vlcMedia = new LibVLCpp::Media( mrl );
}

void        Clip::flushParameters()
{
    //Flushing the args into the media :
    QString     param;
    foreach ( param, m_parameters )
        m_vlcMedia->addOption( param.toStdString().c_str() );
}

void        Clip::addParam( const QString& param )
{
    m_parameters.append( param );
}

void        Clip::setSnapshot( QPixmap* snapshot )
{
    if ( m_snapshot != NULL )
        delete m_snapshot;
    m_snapshot = snapshot;
    emit snapshotChanged();
}

const QPixmap&    Clip::getSnapshot() const
{
    if ( m_snapshot != NULL )
        return *m_snapshot;
    if ( Clip::defaultSnapshot == NULL )
        Clip::defaultSnapshot = new QPixmap( ":/images/images/vlmc.png" );
    return *Clip::defaultSnapshot;
}

const QUuid&        Clip::getUuid() const
{
    return m_uuid;
}

const QFileInfo*    Clip::getFileInfo() const
{
    return m_fileInfo;
}

void                Clip::setLength( qint64 length )
{
    m_length = length;
}

qint64              Clip::getLength() const
{
    return m_length;
}

int                 Clip::getWidth() const
{
    return m_width;
}

void                Clip::setWidth( int width )
{
    m_width = width;
}

int                 Clip::getHeight() const
{
    return m_height;
}

void                Clip::setHeight( int height )
{
    m_height = height;
}

