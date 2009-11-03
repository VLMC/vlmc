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
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QStringList>

#include "MainWindow.h"
#include "MediaListWidget.h"
#include "LibraryWidget.h"
#include "Library.h"
#include "Timeline.h"
#include "About.h"
#include "FileBrowser.h"
#include "WorkflowRenderer.h"
#include "ClipRenderer.h"
#include "UndoStack.h"
#include "ClipProperty.h"
#include "PreviewWidget.h"
#include "PreferenceWidget.h"
#include "ProjectPreferences.h"
#include "ProjectManager.h"
#include "AudioProjectPreferences.h"
#include "VideoProjectPreferences.h"
#include "VLMCPreferences.h"
#include "Import.h"
#include "MediaLibraryWidget.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ), m_renderer( NULL )
{
    m_ui.setupUi( this );
    DockWidgetManager::instance( this )->setMainWindow( this );
    initializeDockWidgets();
    createStatusBar();
    createGlobalPreferences();
    createProjectPreferences();

    // Translations
    connect( this, SIGNAL( translateDockWidgetTitle() ),
             DockWidgetManager::instance(), SLOT( transLateWidgetTitle() ) );

    // Zoom
    connect( m_zoomSlider, SIGNAL( valueChanged( int ) ),
             m_timeline, SLOT( changeZoom( int ) ) );
    connect( m_timeline->tracksView(), SIGNAL( zoomIn() ),
             this, SLOT( zoomIn() ) );
    connect( m_timeline->tracksView(), SIGNAL( zoomOut() ),
             this, SLOT( zoomOut() ) );
    connect( this, SIGNAL( toolChanged( ToolButtons ) ),
             m_timeline, SLOT( setTool( ToolButtons ) ) );

    QSettings s;
    // Restore the geometry
    restoreGeometry( s.value( "MainWindowGeometry" ).toByteArray() );
    // Restore the layout
    restoreState( s.value( "MainWindowState" ).toByteArray() );
}

MainWindow::~MainWindow()
{
    QSettings s;
    // Save the current geometry
    s.setValue( "MainWindowGeometry", saveGeometry() );
    // Save the current layout
    s.setValue( "MainWindowState", saveState() );
    s.sync();

    if ( m_renderer )
        delete m_renderer;
    MetaDataManager::destroyInstance();
    LibVLCpp::Instance::destroyInstance();
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

    MediaLibraryWidget* mediaLibraryWidget = new MediaLibraryWidget( this );

    DockWidgetManager::instance()->addDockedWidget( mediaLibraryWidget,
                                                    tr( "Media Library" ),
                                                    Qt::AllDockWidgetAreas,
                                                    QDockWidget::AllDockWidgetFeatures,

                                                    Qt::LeftDockWidgetArea );
    DockWidgetManager::instance()->addDockedWidget( libraryWidget,
                                      tr( "Old Media Library" ),
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
             SLOT( mediaRemoved( const QUuid& ) ), Qt::DirectConnection );

    connect( libraryWidget->getVideoListWidget(), SIGNAL( selectedClipChanged( Clip* ) ),
              m_clipPreview->getGenericRenderer(), SLOT( setClip( Clip* ) ) );

    connect( libraryWidget->getVideoListWidget(), SIGNAL( itemDoubleClicked( QListWidgetItem* ) ),
                this, SLOT( mediaListItemDoubleClicked( QListWidgetItem* ) ) );

    connect( mediaLibraryWidget, SIGNAL( mediaSelected( Media* ) ),
             m_clipPreview->getGenericRenderer(), SLOT( setMedia( Media* ) ) );

    connect( Library::getInstance(), SIGNAL( mediaRemoved( const QUuid& ) ),
             m_clipPreview->getGenericRenderer(), SLOT( mediaUnloaded( QUuid ) ) );
}

void    MainWindow::on_actionSave_triggered()
{
    QString outputFileName =
            QFileDialog::getSaveFileName( NULL, "Enter the output file name",
                                          QString(), "VLMC project file(*.vlmc)" );
    if ( outputFileName.length() == 0 )
        return ;
    else
    {
        //Project manager will destroy itself.
        QStringList list = outputFileName.split( "." );
        if ( list.at( list.size() - 1 ) != "vlmc" )
        {
            list.append( "vlmc" );
            outputFileName = list.join(".");
        }

        ProjectManager* pm = new ProjectManager( outputFileName );
        pm->saveProject();
    }
}

void    MainWindow::on_actionLoad_Project_triggered()
{
    QString outputFileName =
            QFileDialog::getOpenFileName( NULL, "Enter the output file name",
                                          QString(), "VLMC project file(*.vlmc)" );
    if ( outputFileName.length() == 0 )
        return ;
    else
    {
        //Project manager will destroy itself.
        ProjectManager* pm = new ProjectManager( outputFileName );
        pm->loadProject();
    }
}

void MainWindow::createStatusBar()
{
    // Mouse (default) tool
    QToolButton* mouseTool = new QToolButton( this );
    mouseTool->setCheckable( true );
    mouseTool->setIcon( QIcon( ":/images/mouse" ) );
    m_ui.statusbar->addPermanentWidget( mouseTool );

    // Cut/Split tool
    QToolButton* splitTool = new QToolButton( this );
    splitTool->setCheckable( true );
    splitTool->setIcon( QIcon( ":/images/editcut" ) );
    m_ui.statusbar->addPermanentWidget( splitTool );

    // Group the two previous buttons
    QButtonGroup* toolButtonGroup = new QButtonGroup( this );
    toolButtonGroup->addButton( mouseTool, TOOL_DEFAULT);
    toolButtonGroup->addButton( splitTool, TOOL_CUT );
    toolButtonGroup->setExclusive( true );
    mouseTool->setChecked( true );

    connect( toolButtonGroup, SIGNAL( buttonClicked( int ) ),
             this, SLOT( toolButtonClicked( int ) ) );

    // Spacer
    QWidget* spacer = new QWidget( this );
    spacer->setFixedWidth( 20 );
    m_ui.statusbar->addPermanentWidget( spacer );

    // Zoom IN
    QToolButton* zoomInButton = new QToolButton( this );
    zoomInButton->setIcon( QIcon( ":/images/zoomin" ) );
    m_ui.statusbar->addPermanentWidget( zoomInButton );
    connect( zoomInButton, SIGNAL( clicked() ),
             this, SLOT( zoomIn() ) );

    // Zoom slider
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

    // Zoom Out
    QToolButton* zoomOutButton = new QToolButton( this );
    zoomOutButton->setIcon( QIcon( ":/images/zoomout" ) );
    m_ui.statusbar->addPermanentWidget( zoomOutButton );
    connect( zoomOutButton, SIGNAL( clicked() ),
             this, SLOT( zoomOut() ) );
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
    m_metaDataManager = MetaDataManager::getInstance();
}

void        MainWindow::createGlobalPreferences()
{
    m_globalPreferences = new Settings( this );
    m_globalPreferences->addWidget("VLMC",
                                   new VLMCPreferences( m_globalPreferences ),
                                   "../images/vlmc.png",
                                   "VLMC settings");
    m_globalPreferences->build();
}

void	    MainWindow::createProjectPreferences()
{
    m_projectPreferences = new Settings(  );

    m_projectPreferences->addWidget( "Project",
                                   new ProjectPreferences,
                                   ":/images/images/vlmc",
                                   "Project settings" );
    m_projectPreferences->addWidget( "Video",
                                   new VideoProjectPreferences,
                                   ":/images/images/video",
                                   "Video settings" );
    m_projectPreferences->addWidget( "Audio",
                                   new AudioProjectPreferences,
                                   ":/images/images/audio",
                                   "Audio settings" );
    m_projectPreferences->build();
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

void MainWindow::on_actionTranscode_triggered()
{
    QMessageBox::information( this, tr( "Sorry" ),
                              tr( "This feature is currently disabled." ) );
    //Transcode::instance( this )->exec();
}

void    MainWindow::on_actionRender_triggered()
{
    if ( MainWorkflow::getInstance()->getLengthFrame() <= 0 )
    {
        QMessageBox::warning( NULL, "VLMC Renderer", "There is nothing to render." );
        return ;
    }
    QString outputFileName =
            QFileDialog::getSaveFileName( NULL, "Enter the output file name",
                                          QDir::currentPath(), "Videos(*.avi *.mpg)" );
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

    m_projectPreferences->show();
}

void    MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl( QUrl( "http://vlmc.org" ) );
}

void    MainWindow::on_actionImport_triggered()
{
    //Import* import = new Import( );
    //import->exec();
}

void    MainWindow::zoomIn()
{
    m_zoomSlider->setValue( m_zoomSlider->value() - 1 );
}

void    MainWindow::zoomOut()
{
    m_zoomSlider->setValue( m_zoomSlider->value() + 1 );
}

void    MainWindow::on_actionFullscreen_triggered( bool checked )
{
    if ( checked )
        showFullScreen();
    else
        showNormal();
}

void    MainWindow::registerWidgetInWindowMenu( QDockWidget* widget )
{
    m_ui.menuWindow->addAction( widget->toggleViewAction() );
}

void    MainWindow::mediaListItemDoubleClicked( QListWidgetItem* qItem )
{
    ListViewMediaItem* item = static_cast<ListViewMediaItem*>( qItem );
    ClipProperty* mp = new ClipProperty( item->getClip(), this );
    mp->show();
}

void    MainWindow::toolButtonClicked( int id )
{
    emit toolChanged( (ToolButtons)id );
}

void MainWindow::on_actionBypass_effects_engine_toggled(bool toggled)
{
    EffectsEngine*  ee;

    ee = MainWorkflow::getInstance()->getEffectsEngine();
    if (toggled)
        ee->enable();
    else
       ee->disable();
    return ;
}

void MainWindow::on_actionProject_Preferences_triggered()
{
    m_projectPreferences->show();
}
