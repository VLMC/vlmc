/*****************************************************************************
 * ProjectManager.cpp: Manager the project loading and saving.
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

#include "config.h"

#ifdef WITH_CRASHHANDLER_GUI
    #include "CrashHandler.h"
#endif

#include "Library.h"
#include "MainWorkflow.h"
#include "ProjectManager.h"
#include "SettingsManager.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QtDebug>
#include <QTimer>

#include <errno.h>
#include <signal.h>

#ifdef WITH_CRASHHANDLER
void    ProjectManager::signalHandler( int sig )
{
    signal( sig, SIG_DFL );

    ProjectManager::getInstance()->emergencyBackup();

    #ifdef WITH_CRASHHANDLER_GUI
        CrashHandler* ch = new CrashHandler( sig );
        ::exit( ch->exec() );
    #else
        ::exit( 1 );
    #endif
}
#endif

const QString   ProjectManager::unNamedProject = tr( "<Unnamed project>" );
const QString   ProjectManager::unSavedProject = tr( "<Unsaved project>" );

ProjectManager::ProjectManager() : m_projectFile( NULL ), m_needSave( false )
{
    QSettings s;
    m_recentsProjects = s.value( "RecentsProjects" ).toStringList();

    m_projectName = ProjectManager::unSavedProject;
#ifdef WITH_CRASHHANDLER
    signal( SIGSEGV, ProjectManager::signalHandler );
    signal( SIGFPE, ProjectManager::signalHandler );
    signal( SIGABRT, ProjectManager::signalHandler );
    signal( SIGILL, ProjectManager::signalHandler );
#endif

    connect( this, SIGNAL( projectClosed() ), Library::getInstance(), SLOT( clear() ) );
    connect( this, SIGNAL( projectClosed() ), MainWorkflow::getInstance(), SLOT( clear() ) );

    QVariant val =
            SettingsManager::getInstance()->value( "project/ProjectName", unNamedProject, SettingsManager::XML );
#warning __LINE__ __FILE__ FIXME !!!!
    //connect( val, SIGNAL( changed( QVariant) ), this, SLOT(projectNameChanged(QVariant) ) );

    //Automatic save part :
    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL( timeout() ), this, SLOT( autoSaveRequired() ) );
    QVariant autoSaveEnabled =
            SettingsManager::getInstance()->value( "global/AutomaticBackup", false, SettingsManager::QSett );
#warning __LINE__ __FILE__ FIXME !!!!
    //connect( autoSaveEnabled, SIGNAL( changed( QVariant ) ),
    //         this, SLOT( automaticSaveEnabledChanged( QVariant ) ), Qt::QueuedConnection );
    QVariant autoSaveInterval =
            SettingsManager::getInstance()->value( "global/AutomaticBackupInterval", 5, SettingsManager::QSett );
#warning __LINE__ __FILE__ FIXME !!!!
    //connect( autoSaveInterval, SIGNAL( changed( QVariant ) ),
    //         this, SLOT( automaticSaveIntervalChanged(QVariant) ), Qt::QueuedConnection );
    automaticSaveEnabledChanged( autoSaveEnabled );
}

ProjectManager::~ProjectManager()
{
    // Write uncommited change to the disk
    QSettings s;
    s.sync();

    if ( m_projectFile != NULL )
        delete m_projectFile;
}

bool    ProjectManager::needSave() const
{
    return m_needSave;
}

QStringList ProjectManager::recentsProjects() const
{
    return m_recentsProjects;
}

void    ProjectManager::cleanChanged( bool val )
{
    m_needSave = !val;
    emit projectUpdated( projectName(), val );
}

void    ProjectManager::loadTimeline()
{
    QDomElement     root = m_domDocument->documentElement();

    MainWorkflow::getInstance()->loadProject( root.firstChildElement( "timeline" ) );
    emit projectUpdated( projectName(), true );
}

void    ProjectManager::parseProjectNode( const QDomElement &node )
{
    QDomElement     projectNameNode = node.firstChildElement( "ProjectName" );
    m_projectName = projectNameNode.attribute( "value", ProjectManager::unNamedProject );
}

void    ProjectManager::loadProject( const QString& fileName )
{
    if ( fileName.isEmpty() == true )
        return;

    if ( closeProject() == false )
        return ;

    m_projectFile = new QFile( fileName );

    m_domDocument = new QDomDocument;
    m_projectFile->open( QFile::ReadOnly );
    m_domDocument->setContent( m_projectFile );
    m_projectFile->close();
    m_needSave = false;

    if ( ProjectManager::isBackupFile( fileName ) == false )
        appendToRecentProject( fileName );
    else
    {
        //Delete the project file representation, so the next time the user
        //saves its project, vlmc will ask him where to save it.
        delete m_projectFile;
        m_projectFile = NULL;
    }

    QDomElement     root = m_domDocument->documentElement();

    parseProjectNode( root.firstChildElement( "project" ) );
    connect( Library::getInstance(), SIGNAL( projectLoaded() ), this, SLOT( loadTimeline() ) );
    Library::getInstance()->loadProject( root.firstChildElement( "medias" ) );
#warning FIXME
    //SettingsManager::getInstance()->load( root.firstChildElement( "project" ) );
}

QString  ProjectManager::acquireProjectFileName()
{
    QString fileName =
            QFileDialog::getOpenFileName( NULL, "Enter the output file name",
                                          QString(), "VLMC project file(*.vlmc)" );
    return fileName;
}

bool    ProjectManager::createNewProjectFile( bool saveAs )
{
    if ( m_projectFile == NULL || saveAs == true )
    {
        QString outputFileName =
            QFileDialog::getSaveFileName( NULL, "Enter the output file name",
                                          QString(), "VLMC project file(*.vlmc)" );
        if ( outputFileName.length() == 0 )
            return false;
        if ( m_projectFile != NULL )
            delete m_projectFile;
        if ( outputFileName.endsWith( ".vlmc" ) == false )
            outputFileName += ".vlmc";
        m_projectFile = new QFile( outputFileName );
        appendToRecentProject( outputFileName );
        emit projectUpdated( projectName(), true );
    }
    return true;
}

void    ProjectManager::saveProject( bool saveAs /*= true*/ )
{
    //If the project is still unsaved, or if we want to
    //save the project with a new name
    if ( createNewProjectFile( saveAs ) == false )
        return ;
    __saveProject( m_projectFile->fileName() );
    emit projectSaved();
}

void    ProjectManager::__saveProject( const QString &fileName )
{
    QDomImplementation    implem = QDomDocument().implementation();
    //FIXME: Think about naming the project...
    QString name = "VLMCProject";
    QString publicId = "-//XADECK//DTD Stone 1.0 //EN";
    QString systemId = "http://www-imagis.imag.fr/DTD/stone1.dtd";
    QDomDocument doc(implem.createDocumentType( name, publicId, systemId ) );

    QDomElement rootNode = doc.createElement( "vlmc" );

    Library::getInstance()->saveProject( doc, rootNode );
    MainWorkflow::getInstance()->saveProject( doc, rootNode );
#warning FIXME
    //SettingsManager::getInstance()->saveSettings( "project", doc, rootNode );
#warning FIXME
    //SettingsManager::getInstance()->saveSettings( "keyboard_shortcut", doc, rootNode );

    doc.appendChild( rootNode );

    QFile   file( fileName );
    file.open( QFile::WriteOnly );
    file.write( doc.toString().toAscii() );
    file.close();
}

void    ProjectManager::newProject( const QString &projectName )
{
    if ( closeProject() == false )
        return ;
    m_projectName = projectName;
    emit projectUpdated( this->projectName(), false );
}

bool    ProjectManager::closeProject()
{
    if ( askForSaveIfModified() == false )
        return false;
    if ( m_projectFile != NULL )
    {
        delete m_projectFile;
        m_projectFile = NULL;
    }
    //This one is for the mainwindow, to update the title bar
    emit projectUpdated( ProjectManager::unSavedProject, true );
    //This one is for every part that need to clean something when the project is closed.
    emit projectClosed();
    return true;
}

bool    ProjectManager::askForSaveIfModified()
{
    if ( m_needSave == true )
    {
        QMessageBox msgBox;
        msgBox.setText( tr( "The project has been modified." ) );
        msgBox.setInformativeText( tr( "Do you want to save it ?" ) );
        msgBox.setStandardButtons( QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        msgBox.setDefaultButton(QMessageBox::Save);
        int     ret = msgBox.exec();

        switch ( ret )
        {
            case QMessageBox::Save:
                saveProject();
                break ;
            case QMessageBox::Discard:
                break ;
            case QMessageBox::Cancel:
            default:
                return false ;
        }
    }
    return true;
}

void    ProjectManager::projectNameChanged( const QVariant& name )
{
    m_projectName = name.toString();
    emit projectUpdated( m_projectName, !m_needSave );
}

void    ProjectManager::emergencyBackup()
{
    QString     name;

    if ( m_projectFile != NULL )
    {
        name = m_projectFile->fileName();
        name += "backup";
        __saveProject( name );
    }
    else
    {
       name = QDir::currentPath() + "/unsavedproject.vlmcbackup";
        __saveProject( name );
    }
    QSettings   s;
    s.setValue( "EmergencyBackup", name );
    s.sync();
}

bool    ProjectManager::loadEmergencyBackup()
{
    QSettings   s;
    QString lastProject = s.value( "EmergencyBackup" ).toString();
    if ( QFile::exists( lastProject ) == true )
    {
        loadProject(  lastProject );
        m_needSave = true;
        return true;
    }
    return false;
}

bool    ProjectManager::isBackupFile( const QString& projectFile )
{
    return projectFile.endsWith( "backup" );
}

void    ProjectManager::appendToRecentProject( const QString& projectFile )
{
        // Append the item to the recents list
        m_recentsProjects.removeAll( projectFile );
        m_recentsProjects.prepend( projectFile );
        while ( m_recentsProjects.count() > 15 )
            m_recentsProjects.removeLast();

        QSettings s;
        s.setValue( "RecentsProjects", m_recentsProjects );
        s.sync();
}

void    ProjectManager::automaticSaveEnabledChanged( const QVariant& val )
{
    bool    enabled = val.toBool();

    if ( enabled == true )
    {
        QVariant interval = SettingsManager::getInstance()->value( "global/AutomaticBackupInterval", 5, SettingsManager::QSett );
        m_timer->start( interval.toInt() * 1000 * 60 );
    }
    else
        m_timer->stop();
}

void    ProjectManager::automaticSaveIntervalChanged( const QVariant& val )
{
    QVariant enabled = SettingsManager::getInstance()->value( "global/AutomaticBackup", false, SettingsManager::QSett );

    if ( enabled.toBool() == false )
        return ;
    m_timer->start( val.toInt() * 1000 * 60 );
}

void    ProjectManager::autoSaveRequired()
{
    if ( m_projectFile == NULL )
        return ;
    saveProject( false );
}

QString ProjectManager::projectName() const
{
    if ( m_projectName.isEmpty() == true )
    {
        if ( m_projectFile != NULL )
        {
            QFileInfo       fInfo( *m_projectFile );
            return fInfo.baseName();
        }
        return ProjectManager::unSavedProject;
    }
    return m_projectName;
}
