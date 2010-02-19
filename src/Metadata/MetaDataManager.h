/*****************************************************************************
 * MetaDataWorker.h: MetaDataManager
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

#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include "Singleton.hpp"

#include <QObject>
#include <QQueue>
class   QMutex;
class   Media;
namespace LibVLCpp
{
    class   MediaPlayer;
}

class MetaDataManager : public QObject, public Singleton<MetaDataManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( MetaDataManager );

    public:
        void    computeMediaMetadata( Media* media );
    private:
        MetaDataManager();
        ~MetaDataManager();

        void                    launchComputing( Media *media );

    private:
        QMutex                  *m_computingMutex;
        QQueue<Media*>          m_mediaToCompute;
        bool                    m_computeInProgress;
        LibVLCpp::MediaPlayer   *m_mediaPlayer;
        friend class            Singleton<MetaDataManager>;

    private slots:
        void                    computingCompleted();
        void                    computingFailed( Media* media );

    signals:
        void                    failedToCompute( Media* );
};

#endif //METADATAMANAGER_H
