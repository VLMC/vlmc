/*****************************************************************************
 * QSingleton.hpp : Generic singleton pattern implementation with Qt parent's
 *                  parameter that can be passed to the ctor
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
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


/** \file
  * This file contain the templated singleton.
  * Class/struct to be singletonized just have to
  * iherit from this classe with the class/struct type in template
  * parameter.You have to do few other things, but you know your job,
  * don't you :) ?
  */

#ifndef QSINGLETON_HPP
#define QSINGLETON_HPP

#include <stdlib.h>
#include <QWidget>

template <typename T>
class       QSingleton
{
public:
    static T*      getInstance( QWidget* parent = NULL )
    {
        if ( m_instance == NULL )
            m_instance = new T( parent );
        return m_instance;
    }

    static void    destroyInstance()
    {
        if ( m_instance != NULL )
        {
            delete m_instance;
            m_instance = NULL;
        }
    }
protected:
    QSingleton(){}
    virtual ~QSingleton(){}
    //Not implemented since these methods should *NEVER* been called. If they do, it probably won't compile :)
    QSingleton(const QSingleton<T>&);
    QSingleton<T>&   operator=(const QSingleton<T>&);

private:
    static T*      m_instance;
};

template <typename T>
T*  QSingleton<T>::m_instance = NULL;

#endif // QSINGLETON_HPP
