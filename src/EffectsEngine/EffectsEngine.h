/*****************************************************************************
 * EffectsEngine.h: Main class of the effects engine
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
 * \file EffectsEngine.h
 * \author Vincent Carrubba
 * This file contains the EffectsEngine class definition and declaration
 */

#ifndef EFFECTSENGINE_H_
#define EFFECTSENGINE_H_

#include "EffectNodeFactory.h"

#include <QReadWriteLock>
#include <QtGlobal>

class   EffectNode;
class   LightVideoFrame;

/**
 * \class EffectsEngine
 * \brief This is top-level class of the effects engine
 * It provide a simple set of methods to send audio and video
 * to the effects engine, render it and finally get the result
 * It's thread-safe.
 * The inputs methods, the render and the outputs methods are
 * called a each video frame
 */
class	EffectsEngine
{

 public:

    /**
     * \brief EffectsEngine constructor
     */
    EffectsEngine( void );
    /**
     * \brief EffectsEngine destructor
     */
    ~EffectsEngine();

    /** \brief Patch maker
     * This method instantiates a root EffectNode named "RootNode",
     * creates a mixer effect as subchild in it and connects
     * both to getter to allow audio and video goiny through
     * the effects engine.
     * It's used by the effects engine, when it's enabled.
     */
    void                    makePatch( void );
    /**
     *\brief BypassPatch maker
     * This method instantiates a root EffectNode named "BypassRootNode",
     * it happends the sames things like in void EffectsEngine::makePatch()
     * but the created root EffectNode will never be modified by the GUI.
     * It's used by the effects engine, when it's disabled.
     */
    void                    makeBypassPatch( void );

    /**
    * \brief Enable the effects engine
    * This method enable the effects engine by setting
    * m_enabled to true ( and so tell the effects engine to use the root EffectNode
    * named "RootNode")
    */
    void                    enable( void );
    /**
    * \brief Disable the effects engine
    * This method disable the effects engine by setting
    * m_enabled to false ( and so tell the effects engine to use the root EffectNode
    * named "BypassRootNode")
    */
    void                    disable( void );

    /**
    * \brief Render the audio/video with effects
    * If the patch used by inputs methods is "RootNode", it will call the
    * render method of the root EffectNode called "RootNode"
    * else, it will call the render method of the root EffectNode
    * called "BypassRootNode"
    */
    void                    render( void );

    /**
    * \brief Get the video result of the output with id outId
    * \param outId : this is the id of the video output
    * \return This method return the result track contained in the ouput
    *         with id outId
    * If the patch used by input method is "RootNode", it will get
    * the video frame of the output with id outId of the EffectNode
    * called "RootNode",
    * else, it will do the same thing but with the EffectNode
    * called "BypassRootNode"
    */
    const LightVideoFrame & getVideoOutput( quint32 outId ) const;
    /**
    * \brief Send the video frame in input with id inId
    * \bug If the effects engine enable state is changed beetween
    *      two call of this method, there will be crap
    * \param inId : this is the id of the video input
    * \param frame : this is the frame to send to the input with id inId
    * If the effects engine is enable, it will give the video frame to the
    * input with the id inId of the EffectNode called "RootNode"
    * else, it will give the video frame to the input with the id inId
    * of the EffectNode called "BypassRootNode"
    */
    void                    setVideoInput( quint32 inId, const LightVideoFrame & frame );

private:

    /**
     * \var mutable QReadWriteLock m_rwl
     * This variable is use to permit Thread-safety
     */
    mutable QReadWriteLock  m_rwl;

    /**
     * \var EffectNodeFactory m_enf
     * it permit to manage and store EffectNodes
     */
    EffectNodeFactory       m_enf;

    /**
     * \var EffectNode* m_patch
     * This is the root node/patch used when the effects engine is enabled
     */
    EffectNode*             m_patch;
    /**
     * \var EffectNode* m_bypassPatch
     * This is the root node/patch used when the effects engine is disabled
     */
    EffectNode*             m_bypassPatch;

    /**
     * \var bool m_enabled
     * This var is used to tell the inputs methods to send
     * their datas to m_patch if it's set to true, else
     * to m_bypassPatch
     */
    bool                    m_enabled;
    /**
     * \var EffectNode* m_processedInBypassPatch
     * This var is used to tell the render and the outputs methods
     * to work on m_bypassPatch if it's set to true, else on m_patch
     */
    bool                    m_processedInBypassPatch;

};

#endif // EFFECTSENGINE_H_
