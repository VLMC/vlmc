/*****************************************************************************
 * StackViewController.cpp
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Thomas Boquet <thomas.boquet@gmail.com>
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

#include "StackViewController.h"
#include "ImportController.h"

StackViewController::StackViewController( QWidget* parent, bool enableImport ) :
        QWidget( parent ), m_importButton( NULL ), m_current( 0 )
{
    m_nav     = new StackViewNavController( this );

    m_layout  = new QVBoxLayout;

    m_controllerStack = new QStack<ViewController*>();

    connect( m_nav->previousButton(), SIGNAL( clicked() ),
                     this, SLOT( previous() ) );



    m_layout->addWidget( m_nav );

    if ( enableImport )
    {
        m_importButton = new QPushButton( "Import", parent );
        m_layout->addWidget( m_importButton );
        connect( m_importButton, SIGNAL( clicked() ), this, SIGNAL( importRequired() ) );
    }

    parent->setLayout( m_layout );
}

StackViewController::~StackViewController()
{
    delete m_nav;
    if ( m_importButton != NULL )
        delete m_importButton;
    delete m_layout;
    delete m_controllerStack;
}

void    StackViewController::pushViewController( ViewController* viewController,
                                                 bool animated )
{
    animated = false;

    if ( m_current )
    {
        m_layout->removeWidget( m_current->view() );
        m_current->view()->hide();
        m_controllerStack->push( m_current );

        m_nav->previousButton()->setHidden( false );
        m_nav->previousButton()->setText( "< " + m_current->title() );
    }

    m_current = viewController;
    m_nav->setTitle( m_current->title() );
    m_layout->insertWidget( 1, m_current->view() );
}

void        StackViewController::popViewController( bool animated )
{
    animated = false;

    if ( !m_controllerStack->size() )
        return ;

    m_layout->removeWidget( m_current->view() );
    m_current->view()->hide();
    m_current = m_controllerStack->pop();

    m_nav->setTitle( m_current->title() );
    m_layout->insertWidget( 1, m_current->view() );
    m_current->view()->setHidden( false );

    if ( !m_controllerStack->size() )
        m_nav->previousButton()->setHidden( true );
    else
    {
       m_nav->previousButton()->setText( "< " +
           m_controllerStack->value( m_controllerStack->size() - 1 )->title() );
    }
}

void        StackViewController::previous()
{
    popViewController();
    emit previousButtonPushed();
}

const ViewController*   StackViewController::getCurrentViewController() const
{
    return m_current;
}

