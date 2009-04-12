/*****************************************************************************
 * Library.h: Multimedia library
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

#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QHash>
#include <QUuid>
#include <QFileInfo>

#include "Clip.h"
#include "Singleton.hpp"

class   Library : public QObject, public Singleton<Library>
{
    Q_OBJECT
    Q_DISABLE_COPY( Library );
public:
    Clip*                   getClip( const QUuid& uuid );

private:
    Library();
    QHash<QUuid, Clip*>     m_clips;

public slots:
    void                    newClipLoadingAsked( const QString& filePath );
    void                    removingClipAsked( const QUuid& uuid );

signals:
    void                    newClipLoaded( Clip* );
    void                    clipRemoved( const QUuid& );

    friend class    Singleton<Library>;
};

#endif // LIBRARY_H
