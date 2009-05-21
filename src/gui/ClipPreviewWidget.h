/*****************************************************************************
 * PreviewWidget.cpp: Preview widget
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarrière <geoffroylaca@gmail.com>
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

#ifndef CLIPPREVIEWWIDGET_H
#define CLIPPREVIEWWIDGET_H

#include <QWidget>
#include <QObject>

#include "VLCMediaPlayer.h"
#include "VLCMediaList.h"
#include "Media.h"

class ClipPreviewWidget : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( ClipPreviewWidget )
public:
    explicit ClipPreviewWidget( QWidget* renderWidget );
    virtual ~ClipPreviewWidget();

    void                    startPreview( Media* media );
    void                    setPosition( float newPos );
    void                    togglePlayPause();

private:
    LibVLCpp::MediaPlayer*  m_mediaPlayer;
    bool                    m_clipLoaded;
    bool                    m_videoStopped;

public slots:
    void                    __positionChanged();
    void                    __videoPaused();
    void                    __videoPlaying();
    void                    __endReached();

signals:
    void                    stopped();
    void                    paused();
    void                    playing();
    void                    positionChanged( float );
    void                    endReached();
};

#endif // CLIPPREVIEWWIDGET_H
