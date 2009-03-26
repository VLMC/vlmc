/*****************************************************************************
 * VLCpp.hpp: Generic class to handle an internal vlc type almost transparently.
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

#ifndef VLCPP_H
#define VLCPP_H

#include <cassert>
#include <stdlib.h>

namespace LibVLCpp
{
    template <typename T>
    class   Internal
    {
        public:
            typedef     T*      internalPtr;
            T*                  getInternalPtr()
            {
                assert(this->_internalPtr != NULL);
                return this->_internalPtr;
            }
            operator T*() {return this->_internalPtr;}
        protected:
            Internal() : _internalPtr(NULL) {}

            T*                  _internalPtr;
    };
}

#endif // VLCPP_H
