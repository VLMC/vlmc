/*****************************************************************************
 * SimplePreferences.cpp: generic preferences interface
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
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
#include <QPushButton>
#include <QDialogButtonBox>
#include <QApplication>
#include <QSizePolicy>
#include <QLabel>
#include <QHash>
#include <QIcon>
#include <QLabel>


#include "SimplePreferences.h"
#include "Panel.h"


SimplePreferences::SimplePreferences( QWidget* parent)
    : QWidget(parent),
    m_currentWidget( NULL ),
    m_panel( NULL ),
    m_title( NULL ),
    m_stackedWidgets( 0 )
{
    m_panel = new Panel( this );
    m_stackedWidgets = new QStackedWidget( this );
    connect( m_panel, SIGNAL( changePanel( int ) ),
             SLOT( switchWidget( int ) ) );
    QObject::connect( this, SIGNAL( widgetSwitched( int ) ),
                      m_stackedWidgets, SLOT( setCurrentIndex( int ) ));
}

SimplePreferences::~SimplePreferences()
{
    delete m_panel;
    delete m_title;
    delete m_stackedWidgets;
}


//TODO : see if the widget MUST have a fixed size, or if the window can dynamicaly
//adjust to the size of the biggest Widget.
void        SimplePreferences::addWidget( const QString& name,
                                          QWidget* widget,
                                          const QString& icon,
                                          const QString& label)
{
    m_stackedWidgets->addWidget( widget );

    int idx = m_stackedWidgets->indexOf( widget );
    m_widgets.insert( idx, name );
    m_panel->addButton( label, icon, idx );
    if (m_currentWidget == 0)
    {
        m_currentWidget = widget;
    }
}

void        SimplePreferences::build()
{
    if (m_currentWidget == 0)
        qFatal(  "Can't build the preference panel without an added widget"  );
    QHBoxLayout*        hLayout = new QHBoxLayout( this );
    setLayout( hLayout );
    //TODO : change the size of the widgets to make it look cleaner
    hLayout->addWidget( m_panel );
    hLayout->insertLayout( 1, buildRightHLayout() );
}


QVBoxLayout*    SimplePreferences::buildRightHLayout()
{
    QVBoxLayout*    layout = new QVBoxLayout;
    QFrame*         titleLine = new QFrame;
    QDialogButtonBox*     buttons = new QDialogButtonBox( this );

    m_title = new QLabel( this );
    titleLine->setFrameShape( QFrame::HLine );
    titleLine->setFrameShadow( QFrame::Sunken );

    QFont   labelFont = QApplication::font(this);

    labelFont.setPointSize( labelFont.pointSize() + 6 );
    labelFont.setFamily( "Verdana" );
    m_title->setFont( labelFont );

    buttons->setStandardButtons( QDialogButtonBox::Ok |
                                 QDialogButtonBox::Cancel |
                                 QDialogButtonBox::Apply );

    QString title( m_widgets.value( m_stackedWidgets->indexOf( m_currentWidget ) ) );
    m_title->setText( title );
    layout->addWidget( m_title );
    layout->addWidget( titleLine );
    layout->addWidget( m_stackedWidgets );
    layout->addWidget( buttons );
    return ( layout );
} 

void    SimplePreferences::switchWidget( int widget )
{
    //TODO : Change the title of the preferences shown
    //Hide the current widget and show the new one.
    
    if ( !m_widgets.contains( widget ) )
        return ;
    m_title->setText( m_widgets.value( widget ) );
    emit widgetSwitched( widget );
}

