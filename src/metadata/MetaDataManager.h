/*****************************************************************************
 * MetaDataManager.h: Launch the metadata threads
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


#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <QObject>

#include "Media.h"
#include "Singleton.hpp"

class       MetaDataManager : public QObject, public Singleton<MetaDataManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataManager );

    friend class        Singleton<MetaDataManager>;
    public slots:
        void            newMediaLoaded( Media* );

    private:
        QWidget*        m_renderWidget;

    private:
        MetaDataManager();
        ~MetaDataManager();
};

#endif // METADATAMANAGER_H
