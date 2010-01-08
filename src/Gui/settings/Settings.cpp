/*****************************************************************************
 * Settings.cpp: generic preferences interface
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#include "Settings.h"

#include "PreferenceWidget.h"
#include "SettingsManager.h"
#include "Panel.h"

#include <QDialogButtonBox>
#include <QAbstractButton>
#include <QIcon>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>



Settings::Settings( bool loadDefaults,
                    const QString& name,
                    QWidget* parent,
                    Qt::WindowFlags f )
: QDialog( parent, f ),
    m_currentWidget( NULL ),
    m_defaults( loadDefaults ),
    m_name( name )
{
    setMinimumHeight( 400 );
    setMinimumWidth( 600 );

    m_buttons = new QDialogButtonBox( Qt::Horizontal, this );
    m_buttons->setStandardButtons( QDialogButtonBox::Ok |
                                   QDialogButtonBox::Cancel |
                                   QDialogButtonBox::Apply );

    connect( m_buttons, SIGNAL( clicked( QAbstractButton* ) ),
             this, SLOT( buttonClicked( QAbstractButton* ) ) );


    // Create the layout
    setLayout( buildLayout() );

    SettingsManager::getInstance()->addNewSettingsPart( m_name );

    connect( m_panel, SIGNAL( changePanel( int ) ),
             this, SLOT( switchWidget( int ) ) );
    connect( SettingsManager::getInstance(), SIGNAL( settingsLoaded() ),
             this, SLOT( load() ) );
}

Settings::~Settings()
{
}

void        Settings::addWidget( const QString& name,
        PreferenceWidget* pWidget,
        const QIcon& icon,
        const QString& label )
{
    connect( this, SIGNAL( loadSettings( const QString&, bool ) ) ,
             pWidget, SLOT( loadThemAll( const QString&, bool ) ) );

    // We don't want the widget to be visible
    pWidget->hide();

    // Save the widget name into a property
    pWidget->setProperty( "name", name );

    // Add the widget to the list
    m_pWidgets.append( pWidget );

    // Create a button linked to the widget using its index
    m_panel->addButton( label, icon, m_pWidgets.indexOf( pWidget ) );

    // If this is the first widget, show it by default.
    if ( m_pWidgets.count() == 1 )
        switchWidget( 0 );
}

void        Settings::show( const QString& part )
{
    if ( part == "default" )
        m_defaults = true;
    else
    {
        m_name = part;
        m_defaults = false;
    }
    load();
    switchWidget( 0 );
    QWidget::show();
}

QHBoxLayout*    Settings::buildLayout()
{
    // Create the left panel
    m_panel = new Panel( this );
    m_panel->setMaximumWidth( 130 );

    // Create the master layout
    QHBoxLayout* hLayout = new QHBoxLayout( this );
    hLayout->addWidget( m_panel );

    // Right Sub-layout
    QVBoxLayout* vLayout = new QVBoxLayout;
    m_title = new QLabel( this );
    m_configPanel = new QScrollArea( this );
    m_configPanel->setFrameShape( QFrame::NoFrame );

    // Set the font and text of the panel title
    QFont labelFont = font();
    labelFont.setPointSize( labelFont.pointSize() + 6 );
    labelFont.setFamily( "Verdana" );
    m_title->setFont( labelFont );
    m_title->setText( "Empty" );

    vLayout->addWidget( m_title );
    vLayout->addWidget( m_configPanel );
    vLayout->addWidget( m_buttons );

    hLayout->insertLayout( -1, vLayout );

    return hLayout;
}

void    Settings::save( void )
{
}

void    Settings::buttonClicked( QAbstractButton* button )
{
    bool  save = false;
    bool  hide = false ;

    switch ( m_buttons->standardButton( button ) )
    {
        case QDialogButtonBox::Ok :
            save = true;
            hide = true;
            break;
        case QDialogButtonBox::Cancel :
            hide = true;
            break;
        case QDialogButtonBox::Apply :
            save = true;
            break;
        default :
            break;
    }
    if ( save == true )
    {
        // Ask each widget to save their state
        for ( int i = 0; i < m_pWidgets.count(); ++i )
            m_pWidgets.at( i )->save();

        // then commit
        SettingsManager::getInstance()->commit();
    }
    if ( hide == true ) setVisible( false );
}

void    Settings::switchWidget( int index )
{
    PreferenceWidget* pWidget = m_pWidgets.at( index );

    // This should never happen
    if ( !pWidget ) return;

    QString name( pWidget->property( "name" ).toString() );

    // Set the panel name
    m_title->setText( name );

    // Also set the window title accordingly
    setWindowTitle( tr( "Preferences: %1" ).arg( name ) );

    // If there is already a widget into the QScrollArea take it
    // to avoid its deletion.
    if ( m_configPanel->widget() )
        m_configPanel->takeWidget();

    m_configPanel->setWidget( pWidget );
}

void    Settings::load()
{
    emit loadSettings( m_name, m_defaults );
}
