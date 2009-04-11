/*****************************************************************************
 * Clip.h : Represents a basic container for media informations.
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

#ifndef CLIP_H__
#define CLIP_H__

#include <QList>
#include <QString>
#include <QPixmap>

#include "VLCMedia.h"

/**
  * Represents a basic container for media informations.
  */
class       Clip
{
public:
    Clip( const QString& mrl );
    virtual ~Clip();

    void                loadMedia( const QString& mrl );
    void                addParam( const QString& param );
    void                setupMedia();
    LibVLCpp::Media*    getVLCMedia() { return m_vlcMedia; }

    void                setSnapshot( QPixmap* snapshot );
    const QPixmap&      getSnapshot() const;

protected:
    //TODO: is this really usefull now ?!
    LibVLCpp::Instance*         m_instance;
    LibVLCpp::Media*            m_vlcMedia;
    QString                     m_mrl;
    QList<QString>              m_parameters;
    QPixmap*                    m_snapshot;
};

#endif // MEDIA_H
