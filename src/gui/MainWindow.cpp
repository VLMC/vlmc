/*****************************************************************************
 * MainWindow.cpp: VLMC MainWindow
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#include <QLabel>
#include <QSizePolicy>
#include <QPalette>
#include <QDockWidget>
#include <QFileDialog>
#include <QSlider>

#include "MainWindow.h"
#include "LibraryWidget.h"
#include "Library.h"
#include "Timeline.h"
#include "About.h"
#include "Transcode.h"
#include "FileBrowser.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent )
{
    m_ui.setupUi( this );
    DockWidgetManager::instance( this )->setMainWindow( this );
    m_initializeDockWidgets();
    createStatusBar();

    // Translation
    connect( this, SIGNAL( translateDockWidgetTitle() ),
             DockWidgetManager::instance(), SLOT( transLateWidgetTitle() ) );

    // Zoom
    connect( m_zoomSlider, SIGNAL( valueChanged( int ) ),
             m_timeline, SLOT( changeZoom( int ) ) );
    connect( m_timeline->tracksView(), SIGNAL( zoomIn() ),
             this, SLOT( zoomIn() ) );
    connect( m_timeline->tracksView(), SIGNAL( zoomOut() ),
             this, SLOT( zoomOut() ) );
}

MainWindow::~MainWindow()
{
    if (m_metaDataManager)
        delete m_metaDataManager;
}

void MainWindow::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        emit translateDockWidgetTitle();
        break;
    default:
        break;
    }
}

void        MainWindow::setupLibrary()
{
    //Library part :
    Library*    library = Library::getInstance();

    //GUI part :
    LibraryWidget* libraryWidget = new LibraryWidget( this );

    DockWidgetManager::instance()->addDockedWidget( libraryWidget,
                                  tr( "Media Library" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );

    //Connecting GUI and Frontend :
    connect( libraryWidget,
             SIGNAL( newMediaLoadingAsked(const QString& ) ),
             library,
             SLOT( newMediaLoadingAsked( const QString& ) ) );

    connect( library,
             SIGNAL( newMediaLoaded( Media* ) ),
             libraryWidget,
             SLOT( newMediaLoaded( Media* ) ) );

    connect( libraryWidget,
             SIGNAL( removingMediaAsked( const QUuid& ) ),
             library,
             SLOT( removingMediaAsked( const QUuid& ) ) );

    connect( library,
             SIGNAL( mediaRemoved( const QUuid& ) ),
             libraryWidget,
             SLOT( mediaRemoved( const QUuid& ) ) );
}

void MainWindow::createStatusBar()
{
    m_zoomSlider = new QSlider( this );
    m_zoomSlider->setOrientation( Qt::Horizontal );
    m_zoomSlider->setTickInterval( 1 );
    m_zoomSlider->setSingleStep( 1 );
    m_zoomSlider->setPageStep( 1 );
    m_zoomSlider->setMinimum( 0 );
    m_zoomSlider->setMaximum( 13 );
    m_zoomSlider->setValue( 10 );
    m_zoomSlider->setFixedWidth( 80 );
    m_ui.statusbar->addPermanentWidget( m_zoomSlider );
}

void MainWindow::m_initializeDockWidgets( void )
{
    m_timeline = new Timeline( this );
    m_timeline->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_timeline->show();
    setCentralWidget( m_timeline );

    DockWidgetManager *dockManager = DockWidgetManager::instance();
    dockManager->addDockedWidget( new FileBrowser( this ),
                                  tr( "FileBrowser" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea);


    setupLibrary();

    dockManager->addDockedWidget( new PreviewWidget( this ),
                                  tr( "Preview" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );

    m_metaDataManager = new MetaDataManager();
}

//Private slots definition

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionPreferences_triggered()
{
    Preferences::instance()->exec();
}

void MainWindow::on_actionAbout_triggered()
{
    About::instance()->exec();
}

void MainWindow::on_actionTranscode_File_triggered()
{
    Transcode::instance( this )->exec();
}

void MainWindow::on_actionNew_Project_triggered()
{
    //TODO : clear the library, the timeline, and show the configuration box
    //of the newly created project
}

void MainWindow::on_actionOpen_Project_triggered()
{
    QString path = QFileDialog::getOpenFileName( this, tr( "Choose a project to open" ),
                                                 QDir::currentPath(),
                                                 tr( "VideoLAN Movie Creator file (*.vlmc)" ) );
}

void MainWindow::zoomIn()
{
    m_zoomSlider->setValue( m_zoomSlider->value() - 1 );
}

void MainWindow::zoomOut()
{
    m_zoomSlider->setValue( m_zoomSlider->value() + 1 );
}

void MainWindow::on_actionFullscreen_triggered( bool checked )
{
    if ( checked )
        showFullScreen();
    else
        showNormal();
}
