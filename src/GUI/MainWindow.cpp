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
#include <QUndoView>

#include "MainWindow.h"
#include "MediaListWidget.h"
#include "LibraryWidget.h"
#include "Library.h"
#include "Timeline.h"
#include "About.h"
#include "Transcode.h"
#include "FileBrowser.h"
#include "WorkflowRenderer.h"
#include "ClipRenderer.h"
#include "UndoStack.h"
#include "ClipProperty.h"
#include "PreviewWidget.h"
#include "PreferenceWidget.h"
#include "ProjectPreferences.h"
//#include "AudioDebugSpectrum.h"
#include "AudioProjectPreferences.h"
#include "VideoProjectPreferences.h"
#include "VLMCPreferences.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ), m_renderer( NULL )
{
    m_ui.setupUi( this );
    DockWidgetManager::instance( this )->setMainWindow( this );
    initializeDockWidgets();
    createStatusBar();
    createGlobalPreferences();

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

    //Global Preferences
    QObject::connect( qApp,
                      SIGNAL( aboutToQuit() ),
                      m_globalPreferences,
                      SLOT( deleteLater() ) );
}

MainWindow::~MainWindow()
{
    if ( m_renderer )
        delete m_renderer;
    MetaDataManager::destroyInstance();
    LibVLCpp::Instance::kill();
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
                                  Qt::LeftDockWidgetArea );

    //Connecting GUI and Frontend :
    connect( libraryWidget,
             SIGNAL( newMediaLoadingAsked(const QString& ) ),
             library,
             SLOT( newMediaLoadingAsked( const QString& ) ) );

    connect( library,
             SIGNAL( newClipLoaded( Clip* ) ),
             libraryWidget,
             SLOT( newClipLoaded( Clip* ) ) );

    connect( libraryWidget,
             SIGNAL( removingMediaAsked( const QUuid& ) ),
             library,
             SLOT( removingMediaAsked( const QUuid& ) ) );

    connect( library,
             SIGNAL( mediaRemoved( const QUuid& ) ),
             libraryWidget,
             SLOT( mediaRemoved( const QUuid& ) ) );

    connect( libraryWidget->getVideoListWidget(), SIGNAL( selectedClipChanged( Clip* ) ),
              m_clipPreview->getGenericRenderer(), SLOT( setClip( Clip* ) ) );
    connect( Library::getInstance(), SIGNAL( mediaRemoved( const QUuid& ) ),
             m_clipPreview->getGenericRenderer(), SLOT( mediaUnloaded( QUuid ) ) );
    connect( libraryWidget->getVideoListWidget(), SIGNAL( itemDoubleClicked( QListWidgetItem* ) ),
             this, SLOT( mediaListItemDoubleClicked( QListWidgetItem* ) ) );
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

void MainWindow::initializeDockWidgets( void )
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

    m_clipPreview = new PreviewWidget( new ClipRenderer, this );
    dockManager->addDockedWidget( m_clipPreview,
                                  tr( "Clip Preview" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );

    m_projectPreview = new PreviewWidget( new WorkflowRenderer(), this );
    dockManager->addDockedWidget( m_projectPreview,
                                  tr( "Project Preview" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );

    dockManager->addDockedWidget( UndoStack::getInstance( this ),
                                  tr( "History" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::LeftDockWidgetArea );
    setupLibrary();

//    dockManager->addDockedWidget( new AudioDebugSpectrum( this ),
//                                  tr( "AudioDebugSpectrum" ),
//                                  Qt::AllDockWidgetAreas,
//                                  QDockWidget::AllDockWidgetFeatures,
//                                  Qt::BottomDockWidgetArea );

    m_metaDataManager = MetaDataManager::getInstance();
}

void        MainWindow::createGlobalPreferences()
{
    m_globalPreferences = new Settings(  );
    m_globalPreferences->addWidget("VLMC",
                                   new VLMCPreferences,
                                   "../images/vlmc.png",
                                   "VLMC settings");
    ////For debugging purpose
    //m_globalPreferences->addWidget("Test",
    //                               new QLabel("This is a test"),
    //                               "images/vlmc.png",
    //                               "Test");
    m_globalPreferences->build();
}

//Private slots definition

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionPreferences_triggered()
{
   m_globalPreferences->show();
}

void MainWindow::on_actionAbout_triggered()
{
    About::instance()->exec();
}

void MainWindow::on_actionTranscode_File_triggered()
{
    Transcode::instance( this )->exec();
}

void    MainWindow::on_actionRender_triggered()
{
    if ( MainWorkflow::getInstance()->getLength() <= 0 )
    {
        QMessageBox::warning( NULL, "VLMC Renderer", "There is nothing to render." );
        return ;
    }
    QString outputFileName =
            QFileDialog::getSaveFileName( NULL, "Enter the output file name",
                                          QString(), "Videos(*.avi *.mpg)" );
    if ( outputFileName.length() == 0 )
        return ;
    else
    {
        if ( m_renderer )
            delete m_renderer;
        m_renderer = new WorkflowFileRenderer( outputFileName );
        m_renderer->run();
    }
}

void MainWindow::on_actionNew_Project_triggered()
{
    //TODO : clear the library, the timeline, and show the configuration box
    //of the newly created project

    m_projectPreferences = new Settings(  );
    m_projectPreferences->addWidget("Project",
                                   new ProjectPreferences,
                                   "../images/vlmc.png",
                                   "Project settings");
    m_projectPreferences->addWidget("Video",
                                   new VideoProjectPreferences,
                                   "../images/scalable/video.svg",
                                   "Video settings");
    m_projectPreferences->addWidget("Audio",
                                   new AudioProjectPreferences,
                                   "../images/scalable/audio.svg",
                                   "Audio settings");
    m_projectPreferences->build();
    m_projectPreferences->show();
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

void MainWindow::registerWidgetInViewMenu( QDockWidget* widget )
{
    m_ui.menuView->addAction( widget->toggleViewAction() );
}

void    MainWindow::mediaListItemDoubleClicked( QListWidgetItem* qItem )
{
    ListViewMediaItem* item = static_cast<ListViewMediaItem*>( qItem );
    ClipProperty* mp = new ClipProperty( item->getClip(), this );
    mp->show();
}