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

#include <iostream>

EffectsEngine::EffectsEngine( void ) : m_patch( NULL ), m_bypassPatch( NULL )
{
    //
    //
    // CREATION OF THE PATCH
    //
    //

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


       if ( m_patch->createChild( "libVLMC_MixerEffectPlugin" ) == true )
       {
           tmp = m_patch->getChild( 1 );
           for ( i = 0 ; i < 64; ++i)
               if ( tmp->connectChildStaticVideoInputToParentStaticVideoOutput( ( i + 1 ), ( i + 1 ) ) == false )
                   qDebug() << "The connection of the intput " << i << " of the mixer with the internal " << i << " output of the RootNode failed!";
               else
                   qDebug() << "The connection of the intput " << i << " of the mixer with the internal " << i << " output of the RootNode successed!";
           // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'INTERNAL INPUT DU ROOT NODE
           tmp = m_patch->getChild( 1 );
           if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( 1, 1 ) == false )
               qDebug() << "The connection of the mixer output with the BypassRootNode internal input failed!";
           else
               qDebug() << "The connection of the mixer output with the BypassRootNode internal input successed!";

           //           m_patch->createChild( "libVLMC_BlitInRectangleEffectPlugin" );
           //           m_patch->createChild( "libVLMC_InvertRNBEffectPlugin" );
           // // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'ENTREE 2 DU BLIT
           // tmp = m_patch->getChild( 1 );
           // if ( tmp->connectStaticVideoOutputToStaticVideoInput( 1, 2, "dst" ) == false )
           //     qDebug() << "La connection de la sortie n'as pas reussie HAHA";
           // // RECUP LE BLIT ET CONNECT SA SORTIE 2 A L'INTERNAL INPUT DU ROOT NODE
           // tmp = m_patch->getChild( 2 );
           // qDebug() << "NAME : " << tmp->getInstanceName();
           // if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( "res", 1 ) == false )
           //     qDebug() << "La connection de la sortie n'as pas reussie";
           // // CONNECT SA SORTIE 1 A SA L'ENTREE  1 DE L'INVERSEUR DE BLEU ET DE ROUGE
           // if ( tmp->connectStaticVideoOutputToStaticVideoInput( "aux", 3, 1 ) == false )
           //     qDebug() << "La connection de la sortie n'as pas reussie, MERDE";
           // // CONNECT LA SORTIE DE L'INVERSEUR A L'ENTREE SRC DU BLIT
           // tmp = m_patch->getChild( 3 );
           // if ( tmp->connectStaticVideoOutputToStaticVideoInput( 1, 2, 1 ) == false )
           //     qDebug() << "La connection de la sortie n'as pas reussie, MERDE";
       }
       else
           qDebug() << "There's not the video mixer plugin, so the connection with it cannot exist in the BypassRootNode";
   }

   //
   //
   // CREATION OF THE BYPASS PATCH
   //
   //

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

       if ( m_bypassPatch->createChild( "libVLMC_MixerEffectPlugin" ) == true )
       {
           tmp = m_bypassPatch->getChild( 1 );
           for ( i = 0 ; i < 64; ++i)
               if ( tmp->connectChildStaticVideoInputToParentStaticVideoOutput( ( i + 1 ), ( i + 1 ) ) == false )
                   qDebug() << "The connection of the intput " << i << " of the mixer with the internal " << i << " output of the BypassRootNode failed!";
               else
                   qDebug() << "The connection of the intput " << i << " of the mixer with the internal " << i << " output of the BypassRootNode successed!";
           // RECUP LE MIXER ET CONNECTE SA SORTIE 1 A L'INTERNAL INPUT DU ROOT NODE
           tmp = m_bypassPatch->getChild( 1 );
           if ( tmp->connectChildStaticVideoOutputToParentStaticVideoInput( 1, 1 ) == false )
               qDebug() << "The connection of the mixer output with the BypassRootNode internal input failed!";
           else
               qDebug() << "The connection of the mixer output with the BypassRootNode internal input successed!";
       }
       else
           qDebug() << "There's not the video mixer plugin, so the connection with it cannot exist in the BypassRootNode";
   }
}

EffectsEngine::~EffectsEngine()
{
    if ( m_patch )
        EffectNode::deleteRootNode( "RootNode" );
    if ( m_bypassPatch )
        EffectNode::deleteRootNode( "BypassRootNode" );
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

//
//
//

const LightVideoFrame &
EffectsEngine::getVideoOutput( quint32 outId ) const
{
    return (*m_patch->getInternalStaticVideoInput( outId ));
}

void
EffectsEngine::setVideoInput( quint32 inId, const LightVideoFrame & frame )
{
    (*m_patch->getInternalStaticVideoOutput( inId )) << frame;
}

// BYPASSING

void
EffectsEngine::enable( void )
{
    return ;
}

void
EffectsEngine::disable( void )
{
    return ;
}
