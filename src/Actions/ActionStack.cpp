/*****************************************************************************
 * ActionStack.h: Action stack
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

#include "ActionStack.h"

#include <QDebug>

using namespace Action;

Stack::~Stack()
{
    while ( empty() == false )
    {
        Action::Generic*  act = top();
        delete act;
        pop();
    }
}

void    Stack::addAction( Action::Generic* act )
{
    iterator    it = begin();
    iterator    ite = end();

    while ( it != ite )
    {
        if ( (*it)->isOpposite( act ) == true )
        {
            qDebug() << "is opposite == true";
            delete *it;
            it = erase( it );
            delete act;
        }
        else
            ++it;
    }
    push_back( act );
}
