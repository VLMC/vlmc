/*****************************************************************************
 * MainWindow.cpp: VLMC MainWindow
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
 *          Hugo Beauzee-Luyssen <beauze.h@gmail.com>
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

#include <QSizePolicy>
#include <QDockWidget>
#include <QFileDialog>
#include <QSlider>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>

#include "MainWindow.h"
#include "config.h"
#include "Library.h"
#include "About.h"
#include "ProjectManager.h"
#include "KeyboardShortcut.h"
#include "VlmcDebug.h"

#include "MainWorkflow.h"
#include "WorkflowFileRenderer.h"
#include "WorkflowRenderer.h"
#include "ClipRenderer.h"
#include "EffectsEngine.h"

/* Widgets */
#include "DockWidgetManager.h"
#include "UndoStack.h"
#include "PreviewWidget.h"
#include "MediaLibraryWidget.h"
#include "timeline/Timeline.h"
#include "timeline/TracksView.h"
#include "ImportController.h"

/* Settings / Preferences */
#include "ProjectManager.h"
#include "ProjectWizard.h"
#include "ProjectPreferences.h"
#include "VLMCPreferences.h"
#include "AudioProjectPreferences.h"
#include "VideoProjectPreferences.h"
#include "LanguagePreferences.h"
#include "Settings.h"
#include "SettingsManager.h"

/* VLCpp */
#include "VLCInstance.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ), m_fileRenderer( NULL )
{
    m_ui.setupUi( this );

    qRegisterMetaType<MainWorkflow::TrackType>( "MainWorkflow::TrackType" );
    qRegisterMetaType<MainWorkflow::FrameChangedReason>( "MainWorkflow::FrameChangedReason" );
    qRegisterMetaType<QVariant>( "QVariant" );

    //We only install message handler here cause it uses configuration.
//    VlmcDebug::getInstance()->setup();

    //VLC Instance:
    LibVLCpp::Instance::getInstance( this );

    //Preferences
    initVlmcPreferences();

    //Creating the project manager first (so it can create all the project variables)
    ProjectManager::getInstance();

    // GUI
    DockWidgetManager::instance( this )->setMainWindow( this );
    createGlobalPreferences();
    createProjectPreferences();
    initializeDockWidgets();
    createStatusBar();
    initializeMenuKeyboardShortcut();
#ifdef WITH_CRASHBUTTON
    setupCrashTester();
#endif

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

    connect( ProjectManager::getInstance(), SIGNAL( projectUpdated( const QString&, bool ) ),
             this, SLOT( projectUpdated( const QString&, bool ) ) );

    // Undo/Redo
    connect( UndoStack::getInstance( this ), SIGNAL( canRedoChanged( bool ) ),
             this, SLOT( canRedoChanged( bool ) ) );
    connect( UndoStack::getInstance( this ), SIGNAL( canUndoChanged( bool ) ),
             this, SLOT( canUndoChanged( bool ) ) );
    canRedoChanged( UndoStack::getInstance( this )->canRedo() );
    canUndoChanged( UndoStack::getInstance( this )->canUndo() );


    // Wizard
    m_pWizard = new ProjectWizard( this );
    m_pWizard->setModal( true );

#ifdef WITH_CRASHHANDLER
    QSettings s;
    if ( s.value( "ShowWizardStartup", true ).toBool() &&
         restoreSession() == false )
    {
        m_pWizard->show();
    }
#else
    m_pWizard->show();
#endif
}

MainWindow::~MainWindow()
{
    QSettings s;
    // Save the current geometry
    s.setValue( "MainWindowGeometry", saveGeometry() );
    // Save the current layout
    s.setValue( "MainWindowState", saveState() );
    s.setValue( "CleanQuit", true );
    s.sync();

    if ( m_fileRenderer )
        delete m_fileRenderer;
    delete m_importController;
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

void
MainWindow::initVlmcPreferences()
{
    VLMC_CREATE_PREFERENCE( "keyboard/defaultmode", "n", "Select mode" );
    VLMC_CREATE_PREFERENCE( "keyboard/cutmode", "x", "Cut mode" );
    VLMC_CREATE_PREFERENCE( "keyboard/mediapreview", "Ctrl+Return", "Media preview" );
    VLMC_CREATE_PREFERENCE( "keyboard/renderpreview", "Space", "Render preview" );
    //A bit nasty, but we better use what Qt's providing as default shortcut
    VLMC_CREATE_PREFERENCE( "keyboard/undo", QKeySequence( QKeySequence::Undo ).toString().toLocal8Bit(), "Undo" );
    VLMC_CREATE_PREFERENCE( "keyboard/redo", QKeySequence( QKeySequence::Redo ).toString().toLocal8Bit(), "Redo" );
    VLMC_CREATE_PREFERENCE( "keyboard/help", QKeySequence( QKeySequence::HelpContents ).toString().toLocal8Bit(), "Help" );
    VLMC_CREATE_PREFERENCE( "keyboard/quit", "Ctrl+Q", "Quit" );
    VLMC_CREATE_PREFERENCE( "keyboard/preferences", "Alt+P", "Preferences" );
    VLMC_CREATE_PREFERENCE( "keyboard/fullscreen", "F", "Fullscreen" );
    VLMC_CREATE_PREFERENCE( "keyboard/newproject", QKeySequence( QKeySequence::New ).toString().toLocal8Bit(), "New project" );
    VLMC_CREATE_PREFERENCE( "keyboard/openproject", QKeySequence( QKeySequence::Open ).toString().toLocal8Bit(), "Open a project" );
    VLMC_CREATE_PREFERENCE( "keyboard/save", QKeySequence( QKeySequence::Save ).toString().toLocal8Bit(), "Save" );
    VLMC_CREATE_PREFERENCE( "keyboard/saveas", "Ctrl+Shift+S", "Save as" );
    VLMC_CREATE_PREFERENCE( "keyboard/closeproject", QKeySequence( QKeySequence::Close ).toString().toLocal8Bit(), "Close the project" );
    VLMC_CREATE_PREFERENCE( "keyboard/importmedia", "Ctrl+I", "Import media" );
    VLMC_CREATE_PREFERENCE( "keyboard/renderproject", "Ctrl+R", "Render the project" );

    VLMC_CREATE_PREFERENCE( "global/VLMCLang", "en_US", "The VLMC's UI language" );
}

void        MainWindow::setupLibrary()
{
    //GUI part :

    MediaLibraryWidget* mediaLibraryWidget = new MediaLibraryWidget( this );
    m_importController = new ImportController();

    DockWidgetManager::instance()->addDockedWidget( mediaLibraryWidget,
                                                    tr( "Media Library" ),
                                                    Qt::AllDockWidgetAreas,
                                                    QDockWidget::AllDockWidgetFeatures,
                                                    Qt::LeftDockWidgetArea );
    connect( mediaLibraryWidget, SIGNAL( mediaSelected( Media* ) ),
             m_clipPreview->getGenericRenderer(), SLOT( setMedia( Media* ) ) );

    connect( Library::getInstance(), SIGNAL( mediaRemoved( const QUuid& ) ),
             m_clipPreview->getGenericRenderer(), SLOT( mediaUnloaded( QUuid ) ) );

    connect( mediaLibraryWidget, SIGNAL( importRequired() ), this, SLOT( on_actionImport_triggered() ) );

    connect( m_clipPreview, SIGNAL( addClip( Clip* ) ), mediaLibraryWidget, SLOT( clipAdded( Clip* ) ) );

    connect( mediaLibraryWidget, SIGNAL( previewClipSetted( Clip* ) ),
            m_clipPreview->getGenericRenderer(), SLOT( setClip( Clip* ) ) );
}

void    MainWindow::on_actionSave_triggered()
{
    ProjectManager::getInstance()->saveProject();
}

void    MainWindow::on_actionSave_As_triggered()
{
    ProjectManager::getInstance()->saveProject( true );
}

void    MainWindow::on_actionLoad_Project_triggered()
{
    ProjectManager* pm = ProjectManager::getInstance();
    pm->loadProject( pm->acquireProjectFileName() );
}

void MainWindow::createStatusBar()
{
    // Mouse (default) tool
    QToolButton* mouseTool = new QToolButton( this );
    mouseTool->setAutoRaise( true );
    mouseTool->setCheckable( true );
    mouseTool->setIcon( QIcon( ":/images/mouse" ) );
    m_ui.statusbar->addPermanentWidget( mouseTool );

    // Cut/Split tool
    QToolButton* splitTool = new QToolButton( this );
    splitTool->setAutoRaise( true );
    splitTool->setCheckable( true );
    splitTool->setIcon( QIcon( ":/images/editcut" ) );
    m_ui.statusbar->addPermanentWidget( splitTool );

    // Group the two previous buttons
    QButtonGroup* toolButtonGroup = new QButtonGroup( this );
    toolButtonGroup->addButton( mouseTool, TOOL_DEFAULT);
    toolButtonGroup->addButton( splitTool, TOOL_CUT );
    toolButtonGroup->setExclusive( true );
    mouseTool->setChecked( true );

    //Shortcut part:
    KeyboardShortcutHelper* defaultModeShortcut = new KeyboardShortcutHelper( "keyboard/defaultmode", this );
    KeyboardShortcutHelper* cutModeShortcut = new KeyboardShortcutHelper( "keyboard/cutmode", this );
    connect( defaultModeShortcut, SIGNAL( activated() ), mouseTool, SLOT( click() ) );
    connect( cutModeShortcut, SIGNAL( activated() ), splitTool, SLOT( click() ) );

    connect( toolButtonGroup, SIGNAL( buttonClicked( int ) ),
             this, SLOT( toolButtonClicked( int ) ) );

    // Spacer
    QWidget* spacer = new QWidget( this );
    spacer->setFixedWidth( 20 );
    m_ui.statusbar->addPermanentWidget( spacer );

    // Zoom Out
    QToolButton* zoomOutButton = new QToolButton( this );
    zoomOutButton->setIcon( QIcon( ":/images/zoomout" ) );
    m_ui.statusbar->addPermanentWidget( zoomOutButton );
    connect( zoomOutButton, SIGNAL( clicked() ),
             this, SLOT( zoomOut() ) );

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
    m_zoomSlider->setInvertedAppearance( true );
    m_ui.statusbar->addPermanentWidget( m_zoomSlider );

    // Zoom IN
    QToolButton* zoomInButton = new QToolButton( this );
    zoomInButton->setIcon( QIcon( ":/images/zoomin" ) );
    m_ui.statusbar->addPermanentWidget( zoomInButton );
    connect( zoomInButton, SIGNAL( clicked() ),
             this, SLOT( zoomIn() ) );
}

void MainWindow::initializeDockWidgets( void )
{
    m_renderer = new WorkflowRenderer();
    m_renderer->initializeRenderer();
    m_timeline = new Timeline( m_renderer, this );
    m_timeline->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_timeline->show();
    setCentralWidget( m_timeline );

    DockWidgetManager *dockManager = DockWidgetManager::instance();

    m_clipPreview = new PreviewWidget( new ClipRenderer, this );
    dockManager->addDockedWidget( m_clipPreview,
                                  tr( "Clip Preview" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );
    KeyboardShortcutHelper* clipShortcut = new KeyboardShortcutHelper( "keyboard/mediapreview", this );
    connect( clipShortcut, SIGNAL( activated() ), m_clipPreview, SLOT( on_pushButtonPlay_clicked() ) );

    m_projectPreview = new PreviewWidget( m_renderer, this );
    dockManager->addDockedWidget( m_projectPreview,
                                  tr( "Project Preview" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );
    KeyboardShortcutHelper* renderShortcut = new KeyboardShortcutHelper( "keyboard/renderpreview", this );
    connect( renderShortcut, SIGNAL( activated() ), m_projectPreview, SLOT( on_pushButtonPlay_clicked() ) );

    QDockWidget* dock = dockManager->addDockedWidget( UndoStack::getInstance( this ),
                                  tr( "History" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::LeftDockWidgetArea );
    if ( dock != 0 )
        dock->hide();
    setupLibrary();
}

void        MainWindow::createGlobalPreferences()
{
    m_globalPreferences = new Settings( this );
    m_globalPreferences->addWidget( tr ( "VLMC" ),
                                   new VLMCPreferences( m_globalPreferences ),
                                   QIcon( ":/images/images/vlmc.png" ),
                                   tr ( "VLMC settings" ) );
    m_globalPreferences->addWidget( tr ("Language preferences" ),
                                   new LanguagePreferences( m_globalPreferences ),
                                   QIcon( ":/images/images/vlmc.png" ),
                                   tr ( "Language settings" ) );
    m_globalPreferences->addWidget( tr ("Keyboard" ),
                                     new KeyboardShortcut( m_globalPreferences ),
                                     QIcon( ":/images/keyboard" ),
                                     tr( "Keyboard Settings" ) );
}

void    MainWindow::createProjectPreferences()
{
    m_projectPreferences = new Settings( this );
    m_projectPreferences->addWidget( tr( "Project" ),
                                   new ProjectPreferences,
                                   QIcon( ":/images/images/vlmc.png" ),
                                   tr ( "Project settings" ) );
    m_projectPreferences->addWidget( tr ( "Video" ),
                                   new VideoProjectPreferences,
                                   QIcon( ":/images/images/video.png" ),
                                   tr ( "Video settings" ) );
    m_projectPreferences->addWidget( tr ( "Audio" ),
                                   new AudioProjectPreferences,
                                   QIcon( ":/images/images/audio.png" ),
                                   tr ( "Audio settings" ) );
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
        QMessageBox::warning( NULL, tr ( "VLMC Renderer" ), tr( "There is nothing to render." ) );
        return ;
    }
    else
    {
        m_renderer->killRenderer();
        if ( m_fileRenderer )
            delete m_fileRenderer;
        m_fileRenderer = new WorkflowFileRenderer();
        m_fileRenderer->initializeRenderer();
        m_fileRenderer->run();
    }
}

void MainWindow::on_actionNew_Project_triggered()
{
    //TODO : clear the library, the timeline, and show the configuration box
    //of the newly created project

    m_pWizard->restart();
    m_pWizard->show();
}

void    MainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl( QUrl( "http://vlmc.org" ) );
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

void    MainWindow::closeEvent( QCloseEvent* e )
{
    if ( ProjectManager::getInstance()->askForSaveIfModified() )
        e->accept();
    else
        e->ignore();
}

void    MainWindow::projectUpdated( const QString& projectName, bool savedStatus )
{
    QString title = tr( "VideoLAN Movie Creator" );
    title += " - ";
    title += projectName;
    if ( savedStatus == false )
        title += " *";
    setWindowTitle( title );
}

void    MainWindow::on_actionClose_Project_triggered()
{
    ProjectManager::getInstance()->closeProject();
}

void    MainWindow::on_actionUndo_triggered()
{
    UndoStack::getInstance( this )->undo();
}

void    MainWindow::on_actionRedo_triggered()
{
    UndoStack::getInstance( this )->redo();
}

#define INIT_SHORTCUT( instName, shortcutName, actionInstance  )      \
            QString instName = VLMC_GET_STRING( shortcutName ); \
            KeyboardShortcutHelper* helper##instName = new KeyboardShortcutHelper( shortcutName, m_ui.actionInstance, this ); \
            m_ui.actionInstance->setShortcut( instName );

void    MainWindow::initializeMenuKeyboardShortcut()
{
    INIT_SHORTCUT( help, "keyboard/help", actionHelp );
    INIT_SHORTCUT( quit, "keyboard/quit", actionQuit );
    INIT_SHORTCUT( preferences, "keyboard/preferences", actionPreferences );
    INIT_SHORTCUT( fullscreen, "keyboard/fullscreen", actionFullscreen );
    INIT_SHORTCUT( newProject, "keyboard/newproject", actionNew_Project );
    INIT_SHORTCUT( openProject, "keyboard/openproject", actionLoad_Project );
    INIT_SHORTCUT( save, "keyboard/save", actionSave );
    INIT_SHORTCUT( saveAs, "keyboard/saveas", actionSave_As );
    INIT_SHORTCUT( closeProject, "keyboard/closeproject", actionClose_Project );
    INIT_SHORTCUT( importProject, "keyboard/importmedia", actionImport );
    INIT_SHORTCUT( renderProject, "keyboard/renderproject", actionRender );
    INIT_SHORTCUT( undo, "keyboard/undo", actionUndo );
    INIT_SHORTCUT( redo, "keyboard/redo", actionRedo );
}

#undef INIT_SHORTCUT

void    MainWindow::on_actionCrash_triggered()
{
    //WARNING: read this part at your own risk !!
    //I'm not responsible if you puke while reading this :D
    QString str;
    int test = 1 / str.length();
    Q_UNUSED( test );
}

bool    MainWindow::restoreSession()
{
    QSettings   s;
    bool        fileCreated = false;
    bool        ret = false;

    fileCreated = s.contains( "VlmcVersion" );
    if ( fileCreated == true )
    {
        bool        cleanQuit = s.value( "CleanQuit", false ).toBool();

        // Restore the geometry
        restoreGeometry( s.value( "MainWindowGeometry" ).toByteArray() );
        // Restore the layout
        restoreState( s.value( "MainWindowState" ).toByteArray() );

        if ( cleanQuit == false )
        {
            QMessageBox::StandardButton res = QMessageBox::question( this, tr( "Crash recovery" ), tr( "VLMC didn't closed nicely. Do you wan't to recover your project ?" ),
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
            if ( res == QMessageBox::Yes )
            {
                if ( ProjectManager::getInstance()->loadEmergencyBackup() == true )
                    ret = true;
                else
                    QMessageBox::warning( this, tr( "Can't restore project" ), tr( "VLMC didn't manage to restore your project. We appology for the inconvenience" ) );
            }
        }
    }
    s.setValue( "CleanQuit", false );
    s.sync();
    return ret;
}

void    MainWindow::on_actionImport_triggered()
{
    m_importController->exec();
}

void    MainWindow::canUndoChanged( bool canUndo )
{
    m_ui.actionUndo->setEnabled( canUndo );
}

void    MainWindow::canRedoChanged( bool canRedo )
{
    m_ui.actionRedo->setEnabled( canRedo );
}

#ifdef WITH_CRASHBUTTON
void    MainWindow::setupCrashTester()
{
    QAction* actionCrash = new QAction( this );
    actionCrash->setObjectName( QString::fromUtf8( "actionCrash" ) );
    m_ui.menuTools->addAction( actionCrash );
    actionCrash->setText( QApplication::translate( "MainWindow", "Crash", 0, QApplication::UnicodeUTF8 ) );
    connect( actionCrash, SIGNAL( triggered( bool ) ), this, SLOT( on_actionCrash_triggered() ) );
}
#endif
