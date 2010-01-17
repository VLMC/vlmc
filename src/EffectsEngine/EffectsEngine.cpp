/*****************************************************************************
 * EffectsEngine.cpp: Main class of the effects engine
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
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
        qDebug() << "RootNode creation failed!!!!!!!!!!";
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
            for ( i = 1 ; i <= 64; ++i)
                if ( tmp->connectChildStaticVideoInputToParentStaticVideoOutput( i, i ) == false )
                    qDebug() << "The connection of the input "
                             << i << " of the mixer with the internal "
                             << i << " output of the RootNode failed!";
                else
                    qDebug() << "The connection of the input "
                             << i << " of the mixer with the internal "
                             << i << " output of the RootNode successed!";

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
            else
                qDebug() << "The connection of the mixer output"
                         << "with the second input of the blit successed!";

            // RECUP LE BLIT ET CONNECT SA SORTIE 2 A L'INTERNAL INPUT DU ROOT NODE
            tmp = m_patch->getChild( 2 );
            if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( "res", 1 ) == false )
                qDebug() << "The connection of the second output of the blit"
                         << "with the BypassRootNode internal input failed!";
            else
                qDebug() << "The connection of the second output of the blit"
                         << "with the BypassRootNode internal input successed!";

            // CONNECT SA SORTIE 1 A SA L'ENTREE  1 DE L'INVERSEUR DE BLEU ET DE ROUGE
            if ( tmp->connectStaticVideoOutputToStaticVideoInput( "aux", 3, 1 ) == false )
                qDebug() << "The connection of the first output of the blit"
                         << "with the InvertRNB input failed!";
            else
                qDebug() << "The connection of the first output of the blit"
                         << "with the InvertRNB input successed!";

            // CONNECT LA SORTIE DE L'INVERSEUR A L'ENTREE SRC DU BLIT
            tmp = m_patch->getChild( 3 );
            if ( tmp->connectStaticVideoOutputToStaticVideoInput( 1, 2, 1 ) == false )
                qDebug() << "The connection of the RNBInvert output"
                         << "with the first input of the blit failed!";
            else
                qDebug() << "The connection of the RNBInvert output"
                         << "with the first input of the blit successed!";
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
                else
                    qDebug() << "The connection of the intput "
                             << i << " of the mixer with the internal "
                             << i << " output of the BypassRootNode successed!";

            // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'INTERNAL INPUT DU ROOT NODE
            tmp = m_bypassPatch->getChild( 1 );
            if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( 1, 1 ) == false )
                qDebug() << "The connection of the mixer output"
                         << "with the BypassRootNode internal input failed!";
            else
                qDebug() << "The connection of the mixer output"
                         << "with the BypassRootNode internal input successed!";
        }
        else
            qDebug() << "There's not the video mixer plugin,"
                     << "so the connection with it cannot exist in the BypassRootNode";
    }
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
        m_patch->render();
    else
    {
        configureTransitions();
        m_bypassPatch->render();
    }
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
}

quint32
EffectsEngine::addTransition( quint32 srcTrackId,
                              quint32 dstTrackId,
                              quint32 startFrameId,
                              quint32 stopFrameId )
{
    QWriteLocker  wl( &m_rwl );
    TStart*     tstart;
    TStop*      tstop;

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
    return tstart->m_id;
}

bool
EffectsEngine::moveTransition( quint32 transitionId, quint32 startFrameId, quint32 stopFrameId )
{
    QWriteLocker  wl( &m_rwl );
    TStart*     tstart;
    TStop*      tstop;

    tstart = m_TStartManager.getObject( transitionId );
    tstop = m_TStopManager.getObject( transitionId );
    if ( tstart == NULL || tstop == NULL )
        return false;

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

    // if we're during a transition, think to update the nbSteps in the transition Effect.

    return true;
}

bool
EffectsEngine::removeTransition( quint32 transitionId )
{
    QWriteLocker  wl( &m_rwl );
    TStart*     tstart;
    TStop*      tstop;

    tstart = m_TStartManager.getObject( transitionId );
    tstop = m_TStopManager.getObject( transitionId );
    if ( tstart == NULL || tstop == NULL )
        return false;

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
    return true;
}
