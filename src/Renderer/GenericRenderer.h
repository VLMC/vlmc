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
class   QLabel;

#include "VLCMediaPlayer.h"
#include "MainWorkflow.h"

class   Clip;
class   Media;
class   QUuid;

/**
 *  \class  Common base for every renderer.
 */
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

    /**
     *  \brief  Set the widget used for rendering the output.
     *  \param  renderWidget    The widget to use for render.
     *  \sa     setPreviewLabel( QLabel* )
     */
    void                setRenderWidget( QWidget* renderWidget )
    {
        m_mediaPlayer->setDrawable( renderWidget->winId() );
        m_renderWidget = renderWidget;
    }
    /**
     *  \brief              Sets the label on which a preview will be rendered.
     *
     *  A preview is basically a snapshot of the video, to know what's about to beeing
     *  rendered.
     *  \param          previewLabel    The QLabel to use for print the preview.
     *  \sa             setRenderWidget( QWidget* )
     */
    void                setPreviewLabel( QLabel* previewLabel )
    {
        m_previewLabel = previewLabel;
    }
    /**
     *  \brief          Play or pause the media.
     *
     *  This method is renderer dependant. It has to be implemented in the
     *  underlying renderer implementation.
     *  When this method is called :
     *      - if the render has not started and forcePause is false, the render is started
     *      - if the render has not started and forcePause is true, nothing happens.
     *      - if the render has started and is not paused, the render will pause
     *      - if the render has started, and is paused, the render will unpause if
     *          forcePause is false.
     *  \param  forcePause  Will force the pause if true.
     *  \sa     stop()
     */
    virtual void        togglePlayPause( bool forcePause = false ) = 0;

    /**
     *  \brief Render the next frame
     *  \sa     previousFrame()
     */
    virtual void                    nextFrame() = 0;

    /**
     *  \brief  Render the previous frame
     *  \sa     nextFrame();
     */
    virtual void                    previousFrame() = 0;

    /**
     *  \brief Stop the renderer.
     *  \sa togglePlayPause( bool );
     */
    virtual void                    stop() = 0;

    /**
     * \brief   Return the length in milliseconds
     * \return  The length of the underlying rendered target in milliseconds
     *  \sa     getLength()
     */
    virtual qint64                  getLengthMs() const = 0;

    /**
     *  \brief  Return the current frame number
     *  \return The current frame
     */
    virtual qint64                  getCurrentFrame() const = 0;

    /**
     *  \brief Return the number of frames per second
     *  \return     The current fps
     */
    virtual float                   getFps() const = 0;

    /**
     *  \brief      Returns the currently rendered media.
     *
     *  This method is provided for genericity between renderers, but should never
     *  beeing called from something else than a ClipRenderer
     *  \return     The currently rendered Media
     *  \todo       This method should not be here, as it is not generic at all.
     */
    virtual Media*                  getMedia() const
    {
        return 0;
    }

    /**
     *  \brief      Return the length in frames
     *  \warning    The returned value may not be accurate as it depends on FPS, that
     *              can be badly computed
     *  \return     The length that has to be rendered in frames
     *  \sa         getLengthMs()
     */
    virtual qint64                  getLength() const = 0;

    /**
     *  \brief  Return true if the renderer is paused
     *  \return true if the renderer is paused. false otherwise.
     */
    bool                            isPaused() const
    {
        return m_paused;
    }

    /**
     *  \brief      Return true if the renderer is currently rendering.
     *  \return     true if the renderer is currently rendering. false otherwise.
     *              Note that a paused renderer is still rendering
     *  \sa         isPaused()
     */
    bool                            isRendering() const
    {
        return m_isRendering;
    }

protected:
    /**
     *  \brief  The media player that will be used for rendering
     */
    LibVLCpp::MediaPlayer*          m_mediaPlayer;
    /**
     *  \brief  This flag allows us to know if the render is paused
     *          or not, without using libvlc, especially for the render preview.
     *  If the video is stopped, then this flag will be equal to false
     *  \warning    This is not thread safe.
     *  \sa         isPaused()
     */
    bool                            m_paused;

    /**
     *  \brief  Will be equal to true if a render has been started, even if it paused.
     *  \sa     isRendering()
     */
    bool                            m_isRendering;

    /**
     *  \brief      The QWidget on which we will render.
     *  \sa         setRenderWidget( QWidget* );
     */
    QWidget*                        m_renderWidget;
    /**
     *  \brief      The QLabel used for displaying preview
     *  \sa         setPreviewLabel();
     */
    QLabel*                         m_previewLabel;


public slots:
    /**
     *  \brief      This SLOT has to be called when the render ends.
     */
    virtual void                    __endReached() = 0;
    /**
     *  \brief      Set the Clip to render
     *  \param      clip    The clip to render
     *  \warning    This must only be called from a ClipRenderer implementation
     *  \todo       This method shouldn't be here, as it is not generic at all.
     */
    virtual void                    setClip( Clip* clip ) = 0;
    /**
     *  \brief      Set the Media to render.
     *  \param      media   The media to render.
     *  \warning    This must only be called from a ClipRenderer implementation
     *  \todo       This method shouldn't be here, as it is not generic at all.
     */
    virtual void                    setMedia( Media* media ) = 0;
    /**
     *  \brief      Handle a media unloading. Mainly, this should stop the render
     *              using this media if any.
     *  \param      uuid    The unloaded media uuid
     *  \warning    This must only be called from a ClipRenderer implementation
     *  \todo       This method shouldn't be here, as it is not generic at all.
     */
    virtual void                    mediaUnloaded( const QUuid& uuid ) = 0;
    /**
     *  \brief      This SLOT will be called when the time cursor has changed.
     *
     *  This mainly means that the current rendered frame should change.
     *  \param      newFrame    The new frame to render from.
     */
    virtual void                    previewWidgetCursorChanged( qint64 newFrame ) = 0;


signals:
    /**
     *  \brief  This signal means the render just finished, and has been stoped.
     *  \sa     endReached()
     */
    void                            stopped();
    /**
     *  \brief  Emmited when the render has been paused.
     *  \sa     playing()
     */
    void                            paused();
    /**
     *  \brief  Emmited when the renderer has started to render, and has been unpaused.
     *  \sa     paused()
     */
    void                            playing();
    /**
     *  \brief  Emmited when rendered frame has been changed.
     *  \param  newFrame    The new current frame
     *  \param  reason      The reason for changing frame
     */
    void                            frameChanged( qint64 newFrame,
                                                MainWorkflow::FrameChangedReason reason );
    /**
     *  \brief  Emmited when render end is reached.
     *
     *  This should be emmited just before stopped
     *  \sa     stopped();
     */
    void                            endReached();
};

#endif // GENERICRENDERER_H
