/*****************************************************************************
 * GenericPreviewWidget.h: Describe a common behavior for every preview widget
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

#ifndef GENERICPREVIEWWIDGET_H
#define GENERICPREVIEWWIDGET_H

#include <QObject>
#include <QWidget>

#include "Media.h"
#include "VLCMediaPlayer.h"

class   GenericPreviewWidget : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( GenericPreviewWidget );

public:
    explicit GenericPreviewWidget( QWidget* renderWidget )
    {
        m_mediaPlayer = new LibVLCpp::MediaPlayer();
        m_mediaPlayer->setDrawable( renderWidget->winId() );
    }
    virtual ~GenericPreviewWidget()
    {
        delete m_mediaPlayer;
    }

    virtual void                    startPreview( Media* media ) = 0;
    virtual void                    setPosition( float newPos ) = 0;

protected:
    LibVLCpp::MediaPlayer*          m_mediaPlayer;

public slots:
    virtual void                    __positionChanged() = 0;
    virtual void                    __videoPaused() = 0;
    virtual void                    __videoPlaying() = 0;
    virtual void                    __endReached() = 0;

    //For meta invoking
    virtual void                    togglePlayPause( bool forcePause = false ) = 0;

signals:
    void                    stopped();
    void                    paused();
    void                    playing();
    void                    positionChanged( float );
    void                    endReached();
};

#endif // GENERICPREVIEWWIDGET_H
