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
#include <QLabel>
#include <QHash>
#include <QLabel>


#include "SimplePreferences.h"

/********************************************************************************
                            PreferencesPanel
********************************************************************************/

const int   PreferencesPanel::ICON_HEIGHT = 64;

PreferencesPanel::PreferencesPanel( QWidget* parent )
    : QWidget( parent ),
    m_nbPanel(0)
{
    m_layout = new QVBoxLayout( this );
    m_buttonGroup = new QButtonGroup( this );

    QObject::connect( m_buttonGroup, SIGNAL( buttonClicked( int ) ),
                      this, SLOT( switchPanel( int ) ) );
    setLayout( m_layout );
}

PreferencesPanel::~PreferencesPanel()
{
    delete m_layout;
    delete m_buttonGroup;
}

void    PreferencesPanel::addPanel( const QString& icon,
                                    const QString& label,
                                    const QString& name)
{
    QPushButton* button = new QPushButton( this );

    ++m_nbPanel;
    //button->setIcon(IconPath + icon);
    button->setText( label );
    button->resize( ICON_HEIGHT + 6, ICON_HEIGHT + 6 );
    m_panels.insert( m_nbPanel, name );
    m_buttonGroup->addButton( button, m_nbPanel );
    m_layout->addWidget( button );
}

void    PreferencesPanel::switchPanel( int panel )
{
    if ( !m_panels.contains( panel ) )
        return ;
    emit panelSwitched( m_panels.value( panel ) );
}

/********************************************************************************
                             SimplePreferences
********************************************************************************/

SimplePreferences*  SimplePreferences::m_instance = 0;

SimplePreferences*  SimplePreferences::getInstance()
{
    if ( 0 == SimplePreferences::m_instance )
        SimplePreferences::m_instance = new SimplePreferences();
    return ( SimplePreferences::m_instance );
}

void                SimplePreferences::releaseInstance()
{
    if ( 0 != SimplePreferences::m_instance )
        delete SimplePreferences::m_instance;
    return ;
}

QWidget*    SimplePreferences::getWidget( const QString& name ) const
{
    if ( m_widgets.contains( name ) )
        return ( m_widgets.value( name ) );
    return ( NULL );
}

//TODO : see if the widget MUST have a fixed size, or if the window can dynamicaly
//adjust to the size of the biggest Widget.
void        SimplePreferences::addWidget( const QString& name,
                                          QWidget* widget,
                                          const QString& icon,
                                          const QString& label)
{
    //If there's already a widget with the same name, remove it
    if ( m_widgets.contains( name ) )
       m_widgets.remove( name );
    m_widgets.insert( name, widget );
    m_panel->addPanel( icon, label, name );
}

void        SimplePreferences::build()
{
    if (m_currentWidget == 0)
        qFatal( "Can't build the preference panel without an added widget" );
    QHBoxLayout*        hLayout = new QHBoxLayout( this );
    PreferencesPanel*   prefPanel = new PreferencesPanel( this );
    //TODO : change the size of the widgets to make it look cleaner
    hLayout->addWidget( prefPanel );
    hLayout->addLayout( buildRightHLayout() );
}

SimplePreferences::SimplePreferences()
    : m_currentWidget( NULL ),
    m_panel(NULL)
{
    m_panel = new PreferencesPanel( this );
}

SimplePreferences::~SimplePreferences()
{
    QHash<QString, QWidget*>::iterator     end = m_widgets.end();
    QHash<QString, QWidget*>::iterator     it = m_widgets.begin();
    for ( ; it != end; ++it )
        delete it.value();
}

QVBoxLayout*    SimplePreferences::buildRightHLayout()
{
    QVBoxLayout*    layout = new QVBoxLayout( this );
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

    layout->addWidget( m_title );
    layout->addWidget( m_title );
    layout->addWidget( m_currentWidget );
    layout->addWidget( buttons );
    return ( layout );
} 

void    SimplePreferences::switchWidget( const QString& name )
{
    //TODO : Change the title of the preferences shown
    //Hide the current widget and show the new one.
    
    if ( !m_widgets.contains( name ) )
        return ;
    m_currentWidget->hide();

    QWidget*    wid = m_widgets.value( name );

    m_currentWidget = wid;
    m_currentWidget->show();
}

