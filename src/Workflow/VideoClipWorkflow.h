/*****************************************************************************
 * VideoClipWorkflow.h : Clip workflow. Will extract a single frame from a VLCMedia
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

#ifndef VIDEOCLIPWORKFLOW_H
#define VIDEOCLIPWORKFLOW_H

#include "ClipWorkflow.h"
#include "Clip.h"

class   VideoClipWorkflow : public ClipWorkflow
{
    public:
        VideoClipWorkflow( Clip* clip );
        ~VideoClipWorkflow();
        void*                   getLockCallback();
        void*                   getUnlockCallback();
        virtual void*           getOutput();

    private:
        LightVideoFrame*        m_buffer;
        virtual void            initVlcOutput();
        static void             lock( VideoClipWorkflow* clipWorkflow, void** pp_ret, int size );
        static void             unlock( VideoClipWorkflow* clipWorkflow, void* buffer, int width, int height, int bpp, int size, quint64 pts );
};

#endif // VIDEOCLIPWORKFLOW_H
