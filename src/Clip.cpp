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

Clip::Clip( const QString& mrl )
    : m_vlcMedia( NULL ), m_mrl( mrl ), m_snapshot( NULL )
{
    m_instance = LibVLCpp::Instance::getInstance();
    m_vlcMedia = new LibVLCpp::Media( m_instance, mrl );
    m_uuid = QUuid::createUuid();
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

    m_vlcMedia = new LibVLCpp::Media( m_instance, mrl );
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
    //TODO: check for mem leaks.
    m_snapshot = snapshot;
    emit snapshotChanged();
}

const QPixmap&    Clip::getSnapshot() const
{
    if ( m_snapshot )
        return *m_snapshot;
    //TODO: instanciate this as a static pixmap
    return QPixmap( ":/images/images/vlmc.png" );
}

const QUuid&        Clip::getUuid() const
{
    return m_uuid;
}
