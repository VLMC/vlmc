/*****************************************************************************
 * WorkflowRenderer.cpp: Allow a current workflow preview
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
#include <QThread>

#include "vlmc.h"
#include "WorkflowRenderer.h"
#include "timeline/Timeline.h"
#include "SettingsManager.h"

uint8_t*            WorkflowRenderer::silencedAudioBuffer = NULL;

WorkflowRenderer::WorkflowRenderer() :
            m_mainWorkflow( MainWorkflow::getInstance() ),
            m_stopping( false )
{
    m_actionsMutex = new QMutex;
}

void    WorkflowRenderer::initializeRenderer()
{
    char        videoString[512];
    char        inputSlave[256];
    char        audioParameters[256];
    char        callbacks[64];

    m_waitCond = new QWaitCondition;
    m_renderVideoFrame = new unsigned char[m_mainWorkflow->getWidth()
                                           * m_mainWorkflow->getHeight() * Pixel::NbComposantes];

    m_videoEsHandler = new EsHandler;
    m_videoEsHandler->self = this;
    m_videoEsHandler->type = Video;

    m_audioEsHandler = new EsHandler;
    m_audioEsHandler->self = this;
    m_audioEsHandler->type = Audio;

    m_nbChannels = 2;
    m_rate = 48000;

    sprintf( videoString, "width=%i:height=%i:dar=%s:fps=%s:data=%lld:codec=%s:cat=2:caching=0",
             m_mainWorkflow->getWidth(), m_mainWorkflow->getHeight(), "4/3", "30/1", (qint64)m_videoEsHandler, "RV24" );
    sprintf( audioParameters, "data=%lld:cat=1:codec=s16l:samplerate=%u:channels=%u",
             (qint64)m_audioEsHandler, m_rate, m_nbChannels );
    strcpy( inputSlave, ":input-slave=imem://" );
    strcat( inputSlave, audioParameters );

    m_media = new LibVLCpp::Media( "imem://" + QString( videoString ) );
    m_media->addOption( inputSlave );

    sprintf( callbacks, "imem-get=%lld", (qint64)WorkflowRenderer::lock );
    m_media->addOption( callbacks );
    sprintf( callbacks, ":imem-release=%lld", (qint64)WorkflowRenderer::unlock );
    m_media->addOption( callbacks );

     //Workflow part
    connect( m_mainWorkflow, SIGNAL( mainWorkflowPaused() ), this, SLOT( mainWorkflowPaused() ), Qt::QueuedConnection );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowUnpaused() ), this, SLOT( mainWorkflowUnpaused() ), Qt::QueuedConnection );
    connect( m_mainWorkflow, SIGNAL( mainWorkflowEndReached() ), this, SLOT( __endReached() ) );
    connect( m_mainWorkflow, SIGNAL( frameChanged( qint64, MainWorkflow::FrameChangedReason ) ),
             this, SLOT( __frameChanged( qint64, MainWorkflow::FrameChangedReason ) ) );
}

WorkflowRenderer::~WorkflowRenderer()
{
    stop();

    delete m_videoEsHandler;
    delete m_audioEsHandler;
    delete m_actionsMutex;
    delete m_media;
    delete m_waitCond;
}

int     WorkflowRenderer::lock( void *datas, int64_t *dts, int64_t *pts, unsigned int *flags, size_t *bufferSize, void **buffer )
{
    int             ret = 1;

    EsHandler*  handler = reinterpret_cast<EsHandler*>( datas );
    *dts = -1;
    *flags = 0;
    if ( handler->type == Video )
    {
        ret = lockVideo( handler->self, pts, bufferSize, buffer );
	handler->self->m_mainWorkflow->goToNextFrame( MainWorkflow::VideoTrack );
    }
    else if ( handler->type == Audio )
    {
        ret = lockAudio( handler->self, pts, bufferSize, buffer );
	handler->self->m_mainWorkflow->goToNextFrame( MainWorkflow::AudioTrack );
    }
    else
        qWarning() << "Invalid ES type";
//    qDebug() << "ES Type:" << handler->type << "pts:" << *pts;
    return ret;
}

int     WorkflowRenderer::lockVideo( WorkflowRenderer* self, int64_t *pts, size_t *bufferSize, void **buffer )
{
    qint64 ptsDiff = 0;

    if ( self->m_stopping == false && self->m_paused == false )
    {
        MainWorkflow::OutputBuffers* ret = self->m_mainWorkflow->getSynchroneOutput( MainWorkflow::VideoTrack );
        memcpy( self->m_renderVideoFrame, (*(ret->video))->frame.octets, (*(ret->video))->nboctets );
        self->m_videoBuffSize = (*(ret->video))->nboctets;
        ptsDiff = (*(ret->video))->ptsDiff;
    }
    self->m_pts = *pts = ptsDiff + self->m_pts;
    //qDebug() << "Video pts" << self->m_pts << "diff" << ptsDiff;
    //*pts = qRound64( (float)( self->m_pts * 1000000.0f ) / self->m_outputFps );
    //++self->m_pts;
    *buffer = self->m_renderVideoFrame;
    *bufferSize = self->m_videoBuffSize;
    return 0;
}

int     WorkflowRenderer::lockAudio(  WorkflowRenderer* self, int64_t *pts, size_t *bufferSize, void **buffer )
{
    qint64 ptsDiff;
    if ( self->m_paused == true )
        return 1;
    if ( self->m_stopping == false )
    {
        MainWorkflow::OutputBuffers* ret = self->m_mainWorkflow->getSynchroneOutput( MainWorkflow::AudioTrack );
        self->m_renderAudioSample = ret->audio;
    }
    uint32_t    nbSample;
    if ( self->m_renderAudioSample != NULL )
    {
        nbSample = self->m_renderAudioSample->nbSample;
        *buffer = self->m_renderAudioSample->buff;
        *bufferSize = self->m_renderAudioSample->size;
        ptsDiff = self->m_renderAudioSample->ptsDiff;
    }
    else
    {
        //We set the nbSample to 10ms, which is 1/100 of a sec, so we divide the samplerate
        //by 100.
        nbSample = self->m_rate / self->m_outputFps;
        unsigned int    buffSize = self->m_nbChannels * 2 * nbSample;
        if ( WorkflowRenderer::silencedAudioBuffer == NULL )
            WorkflowRenderer::silencedAudioBuffer = new uint8_t[ buffSize ];
        memset( WorkflowRenderer::silencedAudioBuffer, 0, buffSize );
        *buffer = WorkflowRenderer::silencedAudioBuffer;
        *bufferSize = buffSize;
        ptsDiff = self->m_pts - self->m_audioPts;
    }
    self->m_audioPts = *pts = self->m_audioPts + ptsDiff;
    //qDebug() << "Audio pts" << self->m_audioPts << "diff" << ptsDiff;
    //*pts = self->m_audioPts * 1000000.0f / self->m_rate;
    //self->m_audioPts += nbSample * self->m_nbChannels;
    return 0;
}

void    WorkflowRenderer::unlock( void* datas, size_t, void* )
{
    EsHandler* handler = reinterpret_cast<EsHandler*>( datas );
    handler->self->checkActions();
}

void        WorkflowRenderer::checkActions()
{
    QMutexLocker    lock( m_actionsMutex );

    if ( m_actions.size() == 0 )
        return ;
    while ( m_actions.empty() == false )
    {
        Action::Generic*   act = m_actions.top();
        m_actions.pop();
        act->execute();
        delete act;
    }
}

void        WorkflowRenderer::startPreview()
{
    if ( m_mainWorkflow->getLengthFrame() <= 0 )
        return ;
    m_mediaPlayer->setMedia( m_media );

    //Media player part: to update PreviewWidget
    connect( m_mediaPlayer, SIGNAL( playing() ),    this,   SLOT( __videoPlaying() ), Qt::DirectConnection );
    connect( m_mediaPlayer, SIGNAL( paused() ),     this,   SLOT( __videoPaused() ), Qt::DirectConnection );
    //FIXME:: check if this doesn't require Qt::QueuedConnection
    connect( m_mediaPlayer, SIGNAL( stopped() ),    this,   SLOT( __videoStopped() ) );

    m_mainWorkflow->setFullSpeedRender( false );
    m_mainWorkflow->startRender();
    m_isRendering = true;
    m_paused = false;
    m_stopping = false;
    m_pts = 0;
    m_audioPts = 0;
    m_outputFps = SettingsManager::getInstance()->getValue( "VLMC", "VLMCOutPutFPS" )->get().toDouble();
    m_mediaPlayer->play();
}

void        WorkflowRenderer::nextFrame()
{
    //FIXME: won't work, just compile
    m_mainWorkflow->nextFrame( MainWorkflow::VideoTrack );
}

void        WorkflowRenderer::previousFrame()
{
    m_mainWorkflow->previousFrame( MainWorkflow::VideoTrack );
}

void        WorkflowRenderer::mainWorkflowPaused()
{
    m_paused = true;
    emit paused();
}

void        WorkflowRenderer::mainWorkflowUnpaused()
{
    m_paused = false;
    emit playing();
}

void        WorkflowRenderer::togglePlayPause( bool forcePause )
{
    if ( m_isRendering == false && forcePause == false )
        startPreview();
    else
        internalPlayPause( forcePause );
}

void        WorkflowRenderer::internalPlayPause( bool forcePause )
{
    //If force pause is true, we just ensure that this render is paused... no need to start it.
    if ( m_isRendering == true )
    {
        if ( m_paused == true && forcePause == false )
        {
            Action::Generic*    act = new Action::Unpause( m_mainWorkflow );
            QMutexLocker        lock( m_actionsMutex );
            m_actions.addAction( act );
        }
        else
        {
            if ( m_paused == false )
            {
                Action::Generic*    act = new Action::Pause( m_mainWorkflow );
                QMutexLocker        lock( m_actionsMutex );
                m_actions.addAction( act );
            }
        }
    }
}

void        WorkflowRenderer::stop()
{
    m_isRendering = false;
    m_paused = false;
    m_stopping = true;
    m_mainWorkflow->cancelSynchronisation();
    m_mediaPlayer->stop();
    m_mainWorkflow->stop();
}

qint64      WorkflowRenderer::getCurrentFrame() const
{
    return m_mainWorkflow->getCurrentFrame();
}

qint64      WorkflowRenderer::getLengthMs() const
{
    return m_mainWorkflow->getLengthFrame() / getFps() * 1000;
}

float       WorkflowRenderer::getFps() const
{
    return m_outputFps;
}

void        WorkflowRenderer::removeClip( const QUuid& uuid, uint32_t trackId, MainWorkflow::TrackType trackType )
{
    if ( m_isRendering == true )
    {
        Action::Generic*    act = new Action::RemoveClip( m_mainWorkflow, trackId, trackType, uuid );
        QMutexLocker        lock( m_actionsMutex );
        m_actions.addAction( act );
    }
    else
        m_mainWorkflow->removeClip( uuid, trackId, trackType );
}

void        WorkflowRenderer::addClip( Clip* clip, uint32_t trackId, qint64 startingPos, MainWorkflow::TrackType trackType )
{
    if ( m_isRendering == true )
    {
        Action::Generic*    act = new Action::AddClip( m_mainWorkflow, trackId, trackType, clip, startingPos );
        QMutexLocker        lock( m_actionsMutex );
        m_actions.addAction( act );
    }
    else
        m_mainWorkflow->addClip( clip, trackId, startingPos, trackType );
}

void        WorkflowRenderer::timelineCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame, MainWorkflow::TimelineCursor );
}

void        WorkflowRenderer::previewWidgetCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame, MainWorkflow::PreviewCursor );
}

void        WorkflowRenderer::rulerCursorChanged( qint64 newFrame )
{
    m_mainWorkflow->setCurrentFrame( newFrame, MainWorkflow::RulerCursor );
}

Clip*       WorkflowRenderer::split( Clip* toSplit, Clip* newClip, uint32_t trackId, qint64 newClipPos, qint64 newClipBegin, MainWorkflow::TrackType trackType )
{
    if ( newClip == NULL )
        newClip = new Clip( toSplit, newClipBegin, toSplit->getEnd() );

    if ( m_isRendering == true )
    {
        //adding clip
        //We can NOT call addClip, as it would lock the action lock and then release it,
        //thus potentially breaking the synchrone way of doing this
        Action::Generic*    act = new Action::AddClip( m_mainWorkflow, trackId, trackType, newClip, newClipPos );
        //resizing it
        Action::Generic*    act2 = new Action::ResizeClip( toSplit, toSplit->getBegin(), newClipBegin, true );

        //Push the actions onto the action stack
        QMutexLocker    lock( m_actionsMutex );
        m_actions.addAction( act );
        m_actions.addAction( act2 );
    }
    else
    {
        toSplit->setEnd( newClipBegin, true );
        m_mainWorkflow->addClip( newClip, trackId, newClipPos, trackType );
    }
    return newClip;
}

void    WorkflowRenderer::unsplit( Clip* origin, Clip* splitted, uint32_t trackId, MainWorkflow::TrackType trackType )
{
    if ( m_isRendering == true )
    {
        //removing clip
        Action::Generic*    act = new Action::RemoveClip( m_mainWorkflow, trackId, trackType, splitted->getUuid() );
        //resizing it
        Action::Generic*    act2 = new Action::ResizeClip( origin, origin->getBegin(), splitted->getEnd(), true );
        //Push the actions onto the action stack
        QMutexLocker        lock( m_actionsMutex );
        m_actions.addAction( act );
        m_actions.addAction( act2 );
    }
    else
    {
        m_mainWorkflow->removeClip( splitted->getUuid(), trackId, trackType );
        origin->setEnd( splitted->getEnd(), true );
    }
}

void    WorkflowRenderer::resizeClip( Clip* clip, qint64 newBegin, qint64 newEnd,
                                      qint64 newPos, uint32_t trackId, MainWorkflow::TrackType trackType, bool undoRedoAction /*= false*/ )
{
//    if ( m_isRendering == true )
//    {
//        Action::Generic*    act = new Action::ResizeClip( clip, newBegin, newEnd );
//        Action::Generic*    act2 = new Action::MoveClip( m_mainWorkflow, clip->getUuid(), trackId, trackId, newPos, trackType, undoRedoAction );
//        QMutexLocker        lock( m_actionsMutex );
//        if ( newBegin != clip->getBegin() )
//        {
//            qDebug() << "Resizing to pos:" << newPos;
//            m_actions.addAction( act2 );
//        }
//        qDebug() << "setting boundaries: newEnd:" << newBegin << "newEnd:" << newEnd;
//        m_actions.addAction( act );
//    }
//    else
//    {
        if ( newBegin != clip->getBegin() )
        {
            m_mainWorkflow->moveClip( clip->getUuid(), trackId, trackId, newPos, trackType, undoRedoAction );
        }
        clip->setBoundaries( newBegin, newEnd );

//    }
}

void*   WorkflowRenderer::getLockCallback()
{
    return (void*)&WorkflowRenderer::lock;
}

void*   WorkflowRenderer::getUnlockCallback()
{
    return (void*)&WorkflowRenderer::unlock;
}

/////////////////////////////////////////////////////////////////////
/////SLOTS :
/////////////////////////////////////////////////////////////////////

void        WorkflowRenderer::__endReached()
{
    stop();
    emit endReached();
}

void        WorkflowRenderer::__frameChanged( qint64 frame, MainWorkflow::FrameChangedReason reason )
{
    emit frameChanged( frame, reason );
}

void        WorkflowRenderer::__videoPlaying()
{
    emit playing();
}

void        WorkflowRenderer::__videoStopped()
{
    emit endReached();
}

void        WorkflowRenderer::__videoPaused()
{
    emit paused();
}
