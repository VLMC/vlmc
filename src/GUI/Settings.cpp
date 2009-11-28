/*****************************************************************************
 * Settings.cpp: generic preferences interface
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
#include <QAbstractButton>
#include <QApplication>
#include <QSizePolicy>
#include <QLabel>
#include <QHash>
#include <QIcon>
#include <QLabel>
#include <QtDebug>
#include <QVariant>

#include "PreferenceWidget.h"
#include "SettingsManager.h"
#include "Settings.h"
#include "Panel.h"


Settings::Settings( bool loadDefaults,
                    const QString& name,
                    QWidget* parent,
                    Qt::WindowFlags f )
: QDialog( parent, f ),
    m_currentWidget( NULL ),
    m_defaults( loadDefaults ),
    m_name( name )
{
    m_panel = new Panel( this );
    m_stackedWidgets = new QStackedWidget( this );
    SettingsManager::getInstance()->addNewSettingsPart( m_name );
    connect();
}

Settings::~Settings()
{
}


//TODO : see if the widget MUST have a fixed size, or if the window can dynamicaly
//adjust to the size of the biggest Widget.
void        Settings::addWidget( const QString& name,
        PreferenceWidget* pWidget,
        const QString& icon,
        const QString& label )
{
    m_stackedWidgets->addWidget( pWidget );

    QObject::connect( this,
                      SIGNAL( loadSettings( const QString&, bool ) ) ,
                      pWidget,
                      SLOT( loadThemAll( const QString&, bool ) ) );
    int idx = m_stackedWidgets->indexOf( pWidget );
    m_widgets.insert( idx, name );
    m_pWidgets.push_back( pWidget );
    m_panel->addButton( label, icon, idx );
    if ( !m_currentWidget )
        m_currentWidget = pWidget;
}

void        Settings::build()
{
    if ( !m_currentWidget )
        qFatal( "Can't build the preference panel without having a widget" );

    QHBoxLayout* hLayout = new QHBoxLayout( this );
    setLayout( hLayout );
    //TODO : change the size of the widgets to make it look cleaner
    hLayout->addWidget( m_panel );
    hLayout->insertLayout( 1, buildRightHLayout() );
    load();
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
    QWidget::show();
}

void        Settings::connect( void )
{
    QObject::connect( m_panel,
            SIGNAL( changePanel( int ) ),
            SLOT( switchWidget( int ) ) );
    QObject::connect( this,
            SIGNAL( widgetSwitched( int ) ),
            m_stackedWidgets,
            SLOT( setCurrentIndex( int ) ));
    QObject::connect( SettingsManager::getInstance(),
                        SIGNAL( settingsLoaded() ),
                        this,
                        SLOT( load() ) );
}

QVBoxLayout*    Settings::buildRightHLayout()
{
    QVBoxLayout* layout = new QVBoxLayout;
    QFrame* titleLine = new QFrame( this );
    m_buttons = new QDialogButtonBox( this );

    QObject::connect( m_buttons, SIGNAL( clicked( QAbstractButton* ) ),
            this, SLOT( buttonClicked( QAbstractButton* ) ) );

    m_title = new QLabel( this );
    titleLine->setFrameShape( QFrame::HLine );
    titleLine->setFrameShadow( QFrame::Sunken );

    QFont   labelFont = QApplication::font( this );

    labelFont.setPointSize( labelFont.pointSize() + 6 );
    labelFont.setFamily( "Verdana" );
    m_title->setFont( labelFont );

    m_buttons->setStandardButtons( QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel |
            QDialogButtonBox::Apply );

    QString title( m_widgets.value( m_stackedWidgets->indexOf( m_currentWidget ) ) );
    m_title->setText( title );
    layout->addWidget( m_title );
    layout->addWidget( titleLine );
    layout->addWidget( m_stackedWidgets );
    layout->addWidget( m_buttons );
    return ( layout );
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
        //Save Settings
        PreferenceWidget*		widg;

        foreach( widg, m_pWidgets )
            widg->save();
        SettingsManager::getInstance()->commit();
    }
    if ( hide == true )
        setVisible( false );
}

void    Settings::switchWidget( int widget )
{
    //TODO : Change the title of the preferences shown
    //Hide the current widget and show the new one.

    if ( !m_widgets.contains( widget ) )
        return ;
    m_title->setText( m_widgets.value( widget ) );
    emit widgetSwitched( widget );
}

void    Settings::load()
{
    emit loadSettings( m_name, m_defaults );
}
