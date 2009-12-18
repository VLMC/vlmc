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

#include <QtDebug>
#include "EffectsEngine.h"

// CTOR & DTOR

EffectsEngine::EffectsEngine( void ) : m_patch( NULL ), m_bypassPatch( NULL )
{
   m_inputLock = new QReadWriteLock;

   if ( EffectNode::createRootNode( "RootNode" ) == false )
       qDebug() << "RootNode creation failed!!!!!!!!!!";
   else
   {
       qDebug() << "RootNode successfully created.";
       m_patch = EffectNode::getRootNode( "RootNode" );
       quint32	i;
       EffectNode* tmp;

       m_patch->createChild( "libVLMC_MixerEffectPlugin" );
       m_patch->createChild( "libVLMC_InvertRNBEffectPlugin" );
       for ( i = 0 ; i < 64; ++i)
       {
           m_patch->createStaticVideoInput();
           tmp = m_patch->getChild( 1 );
           if ( tmp->connectChildStaticVideoInputToParentStaticVideoOutput( ( i + 1 ), ( i + 1 ) ) == false )
               qDebug() << "La connection n'a pas reussie";
       }
       m_patch->createStaticVideoOutput();
       tmp = m_patch->getChild( 1 );
       if ( tmp->connectStaticVideoOutputToStaticVideoInput( 1, 2, 1 ) == false )
           qDebug() << "La connection de la sortie n'as pas reussie HAHA";
       tmp = m_patch->getChild( 2 );
       qDebug() << "NAME : " << tmp->getInstanceName();
       if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( 1, 1 ) == false )
           qDebug() << "La connection de la sortie n'as pas reussie";
   }
}

EffectsEngine::~EffectsEngine()
{
    stop();

    if ( m_patch )
        EffectNode::deleteRootNode( "RootNode" );
    delete m_inputLock;
}

//

EffectNode* EffectsEngine::operator->( void )
{
    QReadLocker    rl( &m_rwl );
    return ( m_patch );
}

EffectNode const * EffectsEngine::operator->( void ) const
{
    QReadLocker    rl( &m_rwl );
    return ( m_patch );
}

EffectNode* EffectsEngine::operator*( void )
{
    QReadLocker    rl( &m_rwl );
    return ( m_patch );
}

EffectNode const * EffectsEngine::operator*( void ) const
{
    QReadLocker    rl( &m_rwl );
    return ( m_patch );
}

// MAIN METHOD

void	EffectsEngine::render( void )
{
  QWriteLocker    lock( m_inputLock );
  ( m_effects[0] )->render();
  ( m_effects[1] )->render();
  return ;
}

// BYPASSING

void		EffectsEngine::enable( void )
{
    return ;
}

void		EffectsEngine::disable( void )
{
    return ;
}

// INPUTS & OUTPUTS METHODS


void	EffectsEngine::setInputFrame( LightVideoFrame& frame, quint32 tracknumber )
{
    QWriteLocker    lock( m_inputLock );

    m_videoInputs[tracknumber] = frame;
    return ;
}


LightVideoFrame const &	EffectsEngine::getOutputFrame( quint32 tracknumber ) const
{
  return ( m_videoOutputs[tracknumber] );
}

//
// PRIVATES METHODS
//

// START & STOP

void	EffectsEngine::start( void )
{
  loadEffects();
  patchEffects();
  return ;
}

void	EffectsEngine::stop( void )
{
  unloadEffects();
  return ;
}

// EFFECTS LOADING & UNLOADING

void	EffectsEngine::loadEffects( void )
{
}

void	EffectsEngine::unloadEffects( void )
{
    return ;
}

// EFFECTS PATCHING

void	EffectsEngine::patchEffects( void )
{
}
