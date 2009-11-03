/*****************************************************************************
 * GenericRenderer.h: Describe a common behavior for every renderers
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

#ifndef GENERICRENDERER_H
#define GENERICRENDERER_H

#include <QObject>
#include <QWidget>
#include <QLabel>

#include "Clip.h"
#include "VLCMediaPlayer.h"

class   GenericRenderer : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( GenericRenderer );

public:
    explicit GenericRenderer() :
                m_paused( false ),
                m_isRendering( false )
    {
        m_mediaPlayer = new LibVLCpp::MediaPlayer();
    }
    virtual ~GenericRenderer()
    {
        delete m_mediaPlayer;
    }

    virtual void                    setRenderWidget( QWidget* renderWidget )
    {
        m_mediaPlayer->setDrawable( renderWidget->winId() );
        m_renderWidget = renderWidget;
    }
    virtual void                    setPreviewLabel( QLabel* previewLabel )
    {
        m_previewLabel = previewLabel;
    }
    virtual void                    togglePlayPause( bool forcePause = false ) = 0;
    virtual void                    nextFrame() = 0;
    virtual void                    previousFrame() = 0;
    virtual void                    stop() = 0;
    virtual void                    setPosition( float newPos ) = 0;
    virtual qint64                  getLengthMs() const = 0;
    virtual qint64                  getCurrentFrame() const = 0;
    virtual float                   getFps() const = 0;

    bool                            isPaused() const
    {
        return m_paused;
    }
    bool                            isRendering() const
    {
        return m_isRendering;
    }

protected:
    LibVLCpp::MediaPlayer*          m_mediaPlayer;
    /**
     *  \brief  This flag allows us to know if the render is paused
     *          or not, without using libvlc, especially for the render preview.
     *  If the video is stopped, then this flag will be equal to false
     */
    bool                            m_paused;

    /**
     *  \brief Will be equal to true if a render has been started, even if it paused.
     */
    bool                            m_isRendering;

    QWidget*                        m_renderWidget;
    QLabel*                         m_previewLabel;


public slots:
    virtual void                    __positionChanged() = 0;
    virtual void                    __videoPaused() = 0;
    virtual void                    __videoPlaying() = 0;
    virtual void                    __endReached() = 0;
    virtual void                    setClip( Clip* ) = 0;
    virtual void                    setMedia( Media* ) = 0;
    virtual void                    mediaUnloaded( const QUuid& ) = 0;


signals:
    void                            stopped();
    void                            paused();
    void                            playing();
    void                            positionChanged( float );
    void                            endReached();
};

#endif // GENERICRENDERER_H
