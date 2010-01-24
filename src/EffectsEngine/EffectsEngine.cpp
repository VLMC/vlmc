/*****************************************************************************
 * EffectsEngine.cpp: Main class of the effects engine
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Vincent Carrubba <cyberbouba@gmail.com>
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

/**
 * \file EffectsEngine.cpp
 * \author Vincent Carrubba
 * This file contains the EffectsEngine class implementation
 */

#include "EffectsEngine.h"

#include "EffectNode.h"
#include "EffectNodeFactory.h"
#include "LightVideoFrame.h"
#include "InSlot.hpp"
#include "OutSlot.hpp"

#include <QReadWriteLock>
#include <QtDebug>

EffectsEngine::EffectsEngine( void ) : m_patch( NULL ),
                                       m_bypassPatch( NULL ),
                                       m_enabled( true ),
                                       m_processedInBypassPatch( false )
{
    makePatch();
    makeBypassPatch();
}

EffectsEngine::~EffectsEngine()
{
    if ( m_patch )
        EffectNode::deleteRootNode( "RootNode" );
    if ( m_bypassPatch )
        EffectNode::deleteRootNode( "BypassRootNode" );
}

void
EffectsEngine::makePatch( void )
{
    if ( EffectNode::createRootNode( "RootNode" ) == false )
        qDebug() << "RootNode creation failed !";
    else
    {
        quint32	i;
        EffectNode* tmp;

        qDebug() << "RootNode successfully created!";
        m_patch = EffectNode::getRootNode( "RootNode" );
        // CREATION DU ROOTNODE ET DE SES SLOTS
        for ( i = 0 ; i < 64; ++i)
            m_patch->createStaticVideoInput();
        m_patch->createStaticVideoOutput();


        if ( m_patch->createChild( "Mixer" ) == true )
        {
            tmp = m_patch->getChild( 1 );
            for ( i = 1 ; i <= 64; ++i )
                if ( tmp->connectChildStaticVideoInputToParentStaticVideoOutput( i, i ) == false )
                    qDebug() << "The connection of the input "
                             << i << " of the mixer with the internal "
                             << i << " output of the RootNode failed!";

            // // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'INTERNAL INPUT DU ROOT NODE
            // tmp = m_patch->getChild( 1 );
            // if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( 1, 1 ) == false )
            //     qDebug() << "The connection of the mixer output with the BypassRootNode internal input failed!";
            // else
            //     qDebug() << "The connection of the mixer output with the BypassRootNode internal input successed!";


            m_patch->createChild( "BlitInRectangle" );
            m_patch->createChild( "InvertRNB" );
            // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'ENTREE 2 DU BLIT
            tmp = m_patch->getChild( 1 );
            if ( tmp->connectStaticVideoOutputToStaticVideoInput( 1, 2, "dst" ) == false )
                qDebug() << "The connection of the mixer output"
                         << "with the second input of the blit failed!";

            // RECUP LE BLIT ET CONNECT SA SORTIE 2 A L'INTERNAL INPUT DU ROOT NODE
            tmp = m_patch->getChild( 2 );
            if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( "res", 1 ) == false )
                qDebug() << "The connection of the second output of the blit"
                         << "with the BypassRootNode internal input failed!";

            // CONNECT SA SORTIE 1 A SA L'ENTREE  1 DE L'INVERSEUR DE BLEU ET DE ROUGE
            if ( tmp->connectStaticVideoOutputToStaticVideoInput( "aux", 3, 1 ) == false )
                qDebug() << "The connection of the first output of the blit"
                         << "with the InvertRNB input failed!";

            // CONNECT LA SORTIE DE L'INVERSEUR A L'ENTREE SRC DU BLIT
            tmp = m_patch->getChild( 3 );
            if ( tmp->connectStaticVideoOutputToStaticVideoInput( 1, 2, 1 ) == false )
                qDebug() << "The connection of the RNBInvert output"
                         << "with the first input of the blit failed!";

            // TRANSITIONS

            int i;

            for ( i = 100; i < 100000; i += 100 )
                addTransition( 1, 2, i, i + 50);

        }
        else
            qDebug() << "There's not the video mixer plugin,"
                     << "so the connection with it cannot exist in the RootNode";
    }
}

void
EffectsEngine::makeBypassPatch( void )
{
    if ( EffectNode::createRootNode( "BypassRootNode" ) == false )
        qDebug() << "BypassRootNode creation failed!!!!!!!!!!";
    else
    {
        quint32	i;
        EffectNode* tmp;

        qDebug() << "BypassRootNode successfully created!";

        // CREATION DU BYPASSROOTNODE ET DE SES SLOTS
        m_bypassPatch = EffectNode::getRootNode( "BypassRootNode" );
        for ( i = 0 ; i < 64; ++i)
            m_bypassPatch->createStaticVideoInput();
        m_bypassPatch->createStaticVideoOutput();

        if ( m_bypassPatch->createChild( "Mixer" ) == true )
        {
            tmp = m_bypassPatch->getChild( 1 );
            for ( i = 1 ; i <= 64; ++i)
                if ( tmp->connectChildStaticVideoInputToParentStaticVideoOutput( i, i ) == false )
                    qDebug() << "The connection of the intput "
                             << i << " of the mixer with the internal "
                             << i << " output of the BypassRootNode failed!";

            // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'INTERNAL INPUT DU ROOT NODE
            tmp = m_bypassPatch->getChild( 1 );
            if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( 1, 1 ) == false )
                qDebug() << "The connection of the mixer output"
                         << "with the BypassRootNode internal input failed!";
        }
        else
            qDebug() << "There's not the video mixer plugin,"
                     << "so the connection with it cannot exist in the BypassRootNode";
    }
}

void
EffectsEngine::setCurrentFrameNumber( quint64 nb )
{
    QWriteLocker  wl( &m_rwl );

    m_currentFrameNumber = nb;
}

void
EffectsEngine::setVideoInput( quint32 inId, const LightVideoFrame & frame )
{
    QWriteLocker  wl( &m_rwl );
    if ( m_enabled == true )
    {
        m_processedInBypassPatch = false;
        (*m_patch->getInternalStaticVideoOutput( inId )) << frame;
    }
    else
    {
        m_processedInBypassPatch = true;
        (*m_bypassPatch->getInternalStaticVideoOutput( inId )) << frame;
    }
}

void
EffectsEngine::render( void )
{
    QWriteLocker  wl( &m_rwl );
    if ( m_processedInBypassPatch == false )
    {
        configureTransitions();
        m_patch->render();
    }
    else
        m_bypassPatch->render();
}

const LightVideoFrame &
EffectsEngine::getVideoOutput( quint32 outId ) const
{
    QReadLocker  rl( &m_rwl );

    if ( m_processedInBypassPatch == false )
        return *m_patch->getInternalStaticVideoInput( outId );
    return *m_bypassPatch->getInternalStaticVideoInput( outId );
}

// BYPASSING

void
EffectsEngine::enable( void )
{
    QWriteLocker  wl( &m_rwl );
    m_enabled = true;
}

void
EffectsEngine::disable( void )
{
    QWriteLocker  wl( &m_rwl );
    m_enabled = false;
}


// TEMPORARY TRANSITION WRAPPER

void
EffectsEngine::configureTransitions()
{
    QMap<quint32, TEvent>::iterator it = m_TDoAtNextRender.begin();
    QMap<quint32, TEvent>::iterator end = m_TDoAtNextRender.end();

    qDebug() << m_currentFrameNumber;
    if ( m_TStartTimeline.find( m_currentFrameNumber ) != m_TStartTimeline.end() )
    {
        QMap<quint32, TStart*>::iterator   itStart = (m_TStartTimeline[m_currentFrameNumber]).begin();
        QMap<quint32, TStart*>::iterator   endStart = (m_TStartTimeline[m_currentFrameNumber]).end();
        for ( ; itStart != endStart; ++itStart )
            qDebug() << "TSTART :\n"
                     << "ID : " << itStart.value()->m_id << "\n"
                     << "START FRAME ID : " << itStart.value()->m_startFrameId << "\n"
                     << "SRC TRACK ID : " << itStart.value()->m_srcTrackId << "\n"
                     << "DST TRACK ID : " << itStart.value()->m_dstTrackId << "\n"
                     << "NB STEPS : " << itStart.value()->m_nbSteps;
    }
    else
        qDebug() << "!TSTART";
    if ( m_TStopTimeline.find( m_currentFrameNumber ) != m_TStopTimeline.end() )
    {
        QMap<quint32, TStop*>::iterator    itStop = (m_TStopTimeline[m_currentFrameNumber]).begin();
        QMap<quint32, TStop*>::iterator    endStop = (m_TStopTimeline[m_currentFrameNumber]).end();

        for ( ; itStop != endStop; ++itStop )
            qDebug() << "TSTOP :\n"
                     << "ID : " << itStop.value()->m_id << "\n"
                     << "STOP FRAME ID : " << itStop.value()->m_stopFrameId << "\n"
                     << "SRC TRACK ID : " << itStop.value()->m_srcTrackId << "\n"
                     << "DST TRACK ID : " << itStop.value()->m_dstTrackId;
    }
    else
        qDebug() << "!TSTOP";
    for ( ; it != end; ++it )
        switch ( it.value().m_type )
        {
            qDebug() << "TEVENT :\n";
        case TEvent::DEL:
            {
                qDebug() << "TYPE : DEL\n"
                         << "ID : " << it.value().m_id;
                break;
            }
        case TEvent::ADD:
            {
                qDebug() << "TYPE : ADD\n"
                         << "ID : " << it.value().m_id << "\n"
                         << "CURRENT STEP : " <<  it.value().m_currentStep << "\n"
                         << "NB STEPS : " <<  it.value().m_nbSteps;
                break;
            }
        case TEvent::UPDATE:
            {
                qDebug() << "TYPE : UPDATE\n"
                         << "ID : " << it.value().m_id << "\n"
                         << "CURRENT STEP : " <<  it.value().m_currentStep << "\n"
                         << "NB STEPS : " <<  it.value().m_nbSteps;
                break;
            }
        case TEvent::PATCH:
            {
                qDebug() << "TYPE : PATCH\n"
                         << "ID : " << it.value().m_id << "\n"
                         << "CURRENT STEP : " <<  it.value().m_currentStep << "\n"
                         << "NB STEPS : " <<  it.value().m_nbSteps;
                break;
            }
        case TEvent::UNPATCH:
            {
                qDebug() << "TYPE : UNPATCH\n"
                         << "ID : " << it.value().m_id;
                break;
            }
        }
}

quint32
EffectsEngine::addTransition( quint32 srcTrackId,
                              quint32 dstTrackId,
                              quint64 startFrameId,
                              quint64 stopFrameId )
{
    QWriteLocker  wl( &m_rwl );
    TStart*     tstart;
    TStop*      tstop;
    quint32     i;

    m_TStartManager.createObject();
    m_TStopManager.createObject();
    tstart = m_TStartManager.getObject( m_TStartManager.getNBObjects() );
    tstop = m_TStopManager.getObject( m_TStopManager.getNBObjects() );

    tstart->m_startFrameId = startFrameId;
    tstart->m_nbSteps = stopFrameId - startFrameId;
    tstart->m_srcTrackId = srcTrackId;
    tstart->m_dstTrackId = dstTrackId;

    tstop->m_stopFrameId = stopFrameId;
    tstop->m_srcTrackId = srcTrackId;
    tstop->m_dstTrackId = dstTrackId;
    m_TStartTimeline[startFrameId][tstart->m_id] = tstart;
    m_TStopTimeline[stopFrameId][tstop->m_id] = tstop;

    if ( ( m_currentFrameNumber <= stopFrameId ) &&
         ( m_currentFrameNumber >= startFrameId ) )
    {
        m_TDoAtNextRender[tstart->m_id].m_type = TEvent::ADD;
        m_TDoAtNextRender[tstart->m_id].m_id = tstart->m_id;
        m_TDoAtNextRender[tstart->m_id].m_currentStep = m_currentFrameNumber - startFrameId;
        m_TDoAtNextRender[tstart->m_id].m_currentStep = stopFrameId - startFrameId;
    }

    return tstart->m_id;
}

bool
EffectsEngine::moveTransition( quint32 transitionId, quint64 startFrameId, quint64 stopFrameId )
{
    QWriteLocker  wl( &m_rwl );
    TStart*     tstart;
    TStop*      tstop;
    quint64     oldStartFrameId;
    quint64     oldStopFrameId;

    tstart = m_TStartManager.getObject( transitionId );
    tstop = m_TStopManager.getObject( transitionId );
    if ( tstart == NULL || tstop == NULL )
        return false;

    oldStartFrameId = tstart->m_startFrameId;
    oldStopFrameId = tstop->m_stopFrameId;

    if ( tstart->m_startFrameId != startFrameId )
    {
        m_TStartTimeline[tstart->m_startFrameId].
            erase( m_TStartTimeline[tstart->m_startFrameId].find( tstart->m_id ) );
        if ( m_TStartTimeline[tstart->m_startFrameId].isEmpty() == true )
            m_TStartTimeline.erase( m_TStartTimeline.find( tstart->m_startFrameId ) );
        m_TStartTimeline[startFrameId][transitionId] = tstart;
    }

    if ( tstop->m_stopFrameId != stopFrameId )
    {
        m_TStopTimeline[tstop->m_stopFrameId].
            erase( m_TStopTimeline[tstop->m_stopFrameId].find( tstop->m_id ) );
        if ( m_TStopTimeline[tstop->m_stopFrameId].isEmpty() == true )
            m_TStopTimeline.erase( m_TStopTimeline.find( tstop->m_stopFrameId ) );
        m_TStopTimeline[startFrameId][transitionId] = tstop;
    }

    tstart->m_startFrameId = startFrameId;
    tstart->m_nbSteps = stopFrameId - startFrameId;
    tstop->m_stopFrameId = stopFrameId;


    if ( ( m_currentFrameNumber <= oldStopFrameId ) &&
         ( m_currentFrameNumber >= oldStartFrameId ) )
    {
        if ( ( m_currentFrameNumber <= stopFrameId ) &&
             ( m_currentFrameNumber >= startFrameId ) )
        {
            // si la transition etait pendant la frame courante et le reste,
            // alors il faut mettre a jour les parametres de la transition
            m_TDoAtNextRender[transitionId].m_type = TEvent::UPDATE;
            m_TDoAtNextRender[transitionId].m_id = tstart->m_id;
            m_TDoAtNextRender[transitionId].m_currentStep = m_currentFrameNumber - startFrameId;
            m_TDoAtNextRender[transitionId].m_nbSteps = stopFrameId - startFrameId;
        }
        else
        {
            // si le deplacement place la transition hors de la frame courrante,
            // alors qu'elle y etait precedement, il faut deconnecter l'effet
            m_TDoAtNextRender[transitionId].m_type = TEvent::UNPATCH;
            m_TDoAtNextRender[transitionId].m_id = tstart->m_id;
        }
    }
    else
    {
        // si le deplacement place la transition pendant la frame courrante,
        // alors il faut connecter l'effet et le configurer
        m_TDoAtNextRender[transitionId].m_type = TEvent::PATCH;
        m_TDoAtNextRender[transitionId].m_id = tstart->m_id;
        m_TDoAtNextRender[transitionId].m_currentStep = m_currentFrameNumber - startFrameId;
        m_TDoAtNextRender[transitionId].m_nbSteps = stopFrameId - startFrameId;
    }
    return true;
}

bool
EffectsEngine::removeTransition( quint32 transitionId )
{
    QWriteLocker  wl( &m_rwl );
    TStart*     tstart;
    TStop*      tstop;
    quint64     startFrameId;
    quint64     stopFrameId;

    tstart = m_TStartManager.getObject( transitionId );
    tstop = m_TStopManager.getObject( transitionId );
    if ( tstart == NULL || tstop == NULL )
        return false;

    startFrameId = tstart->m_startFrameId;
    stopFrameId = tstop->m_stopFrameId;
    m_TStartTimeline[tstart->m_startFrameId].
        erase( m_TStartTimeline[tstart->m_startFrameId].find( tstart->m_id ) );
    if ( m_TStartTimeline[tstart->m_startFrameId].isEmpty() == true )
        m_TStartTimeline.erase( m_TStartTimeline.find( tstart->m_startFrameId ) );
    m_TStopTimeline[tstop->m_stopFrameId].
        erase( m_TStopTimeline[tstop->m_stopFrameId].find( tstop->m_id ) );
    if ( m_TStopTimeline[tstop->m_stopFrameId].isEmpty() == true )
        m_TStopTimeline.erase( m_TStopTimeline.find( tstop->m_stopFrameId ) );

    m_TStartManager.deleteObject( transitionId );
    m_TStopManager.deleteObject( transitionId );

    if ( ( m_currentFrameNumber <= stopFrameId ) &&
         ( m_currentFrameNumber >= startFrameId ) )
    {
        m_TDoAtNextRender[transitionId].m_type = TEvent::DEL;
        m_TDoAtNextRender[transitionId].m_id = transitionId;
    }
    return true;
}
