/*****************************************************************************
 * ImportController.cpp
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
 *          Thomas Boquet <thomas.boquet@gmail.com>
 *          Clement Chavance <chavance.c@gmail.com>
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

#include "ui_ImportController.h"

#include "ClipRenderer.h"
#include "ImportController.h"
#include "Library.h"

#include <QPalette>
#include <QSettings>
#include <QTime>

ImportController::ImportController(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ImportController),
    m_clipListController( 0 ),
    m_controllerSwitched( false )
{
    m_ui->setupUi(this);
    m_preview = new PreviewWidget( new ClipRenderer, m_ui->previewContainer );
    m_stackNav = new StackViewController( m_ui->stackViewContainer, false );
    m_mediaListController = new ImportMediaListController( m_stackNav );
    m_tag = new TagWidget( m_ui->tagContainer, 6 );
    m_filesModel = new QDirModel();
    m_stackNav->pushViewController( m_mediaListController );

    QStringList filters;
    filters << "*.mov" << "*.avi" << "*.mkv" << "*.mpg" << "*.mpeg" << "*.wmv" << "*.mp4"
            << "*.ogg" << "*.ogv";
    filters << "*.mp3" << "*.oga" << "*.flac" << "*.aac" << "*.wav";
    filters << "*.gif" << "*.png" << "*.jpg";
    m_filesModel->setFilter( QDir::AllDirs | QDir::Files | QDir::Readable | QDir::Drives |
                             QDir::NoDotAndDotDot );
    m_filesModel->sort( 2, Qt::AscendingOrder );
    m_filesModel->sort( 0, Qt::AscendingOrder );
    m_filesModel->setNameFilters( filters );

    Library::getInstance()->setFilter( filters );

    restoreCurrentPath();

    m_ui->treeView->setModel( m_filesModel );
    m_ui->treeView->setRootIndex( m_filesModel->index( QDir::rootPath() ) );
    m_ui->treeView->setCurrentIndex( m_filesModel->index( m_currentlyWatchedDir ) );
    m_ui->treeView->setExpanded( m_ui->treeView->currentIndex() , true );
    m_ui->treeView->setColumnHidden( 1, true );
    m_ui->treeView->setColumnHidden( 2, true );
    m_ui->treeView->setColumnHidden( 3, true );
    m_ui->forwardButton->setEnabled( true );

    m_fsWatcher = new QFileSystemWatcher();
    m_fsWatcher->addPath( m_currentlyWatchedDir );

    connect( m_fsWatcher, SIGNAL( directoryChanged( QString ) ),
             m_filesModel, SLOT( refresh() ) );
    connect( m_ui->treeView, SIGNAL( clicked( QModelIndex ) ),
             this, SLOT( treeViewClicked( QModelIndex ) ) );
    connect( m_ui->treeView, SIGNAL( doubleClicked( QModelIndex ) ),
             this, SLOT( treeViewDoubleClicked( QModelIndex ) ) );
    connect( m_ui->forwardButton, SIGNAL( clicked() ),
             this, SLOT( forwardButtonClicked() ) );

    connect( Library::getInstance(), SIGNAL( newMediaLoaded( const QUuid& ) ),
             this, SLOT( newMediaLoaded( const QUuid& ) ) );
    connect( Library::getInstance(), SIGNAL( updateMediaRequested( const QUuid& ) ),
             this, SLOT( updateMediaRequested( const QUuid& ) ) );

    connect( this, SIGNAL( mediaSelected( Media* ) ),
             m_preview->getGenericRenderer(), SLOT( setMedia( Media* ) ) );
    connect( this, SIGNAL( mediaSelected( Media* ) ),
             m_tag, SLOT( mediaSelected( Media* ) ) );

    connect( m_mediaListController, SIGNAL( mediaSelected( const QUuid& ) ),
             this, SLOT( mediaSelection( const QUuid& ) ) );
    connect( m_mediaListController, SIGNAL( mediaDeleted( const QUuid& ) ),
             this, SLOT( mediaDeletion( const QUuid& ) ) );

    connect( m_mediaListController, SIGNAL( showClipListAsked( const QUuid& ) ),
             this, SLOT( showClipList( const QUuid& ) ) );
    connect( m_preview, SIGNAL( addClip( Clip* ) ),
             m_mediaListController, SLOT( clipAdded( Clip* ) ) ); //StackViewController
    connect( m_stackNav, SIGNAL( previousButtonPushed() ),
             this, SLOT( restoreContext() ) );
}

ImportController::~ImportController()
{
    delete m_ui;
    delete m_stackNav;
    delete m_tag;
}

void
ImportController::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
        case QEvent::LanguageChange:
            m_ui->retranslateUi( this );
            break;
        default:
            break;
    }
}

void
ImportController::mediaSelection( const QUuid& uuid )
{
    if ( m_mediaListController->contains( uuid ) )
    {
        if ( !m_currentUuid.isNull() && m_mediaListController->contains( m_currentUuid ) )
            m_mediaListController->cell( m_currentUuid )->setPalette( this->palette() );
        QPalette p = m_mediaListController->cell( uuid )->palette();
        p.setColor( QPalette::Window, QColor( Qt::darkBlue ) );
        m_mediaListController->cell( uuid )->setPalette( p );

        setUIMetaData( Library::getInstance()->temporaryMedia( uuid ) );
        if ( uuid != NULL && uuid != m_currentUuid )
            m_preview->stop();
        m_currentUuid = uuid;
        emit mediaSelected( Library::getInstance()->temporaryMedia( uuid ) );
    }
}

void
ImportController::clipSelection( const QUuid& uuid )
{
    if ( !m_currentUuid.isNull() && !m_controllerSwitched )
        m_clipListController->cell( m_currentUuid )->setPalette( palette() );
    else
        m_controllerSwitched = false;
    QPalette p = m_clipListController->cell( uuid )->palette();
    p.setColor( QPalette::Window, QColor( Qt::darkBlue ) );
    m_clipListController->cell( uuid )->setPalette( p );
    Clip*   clip;
    foreach(QUuid id, Library::getInstance()->temporaryMedias()->keys() )
    {
        Media* media = Library::getInstance()->temporaryMedias()->value( id );
        if ( ( clip = media->clip( uuid ) ) != 0 )
            break;
    }
    if ( clip == 0 )
        return ;
    setUIMetaData( clip );
    if ( uuid != m_currentUuid )
        m_preview->stop();
    emit clipSelected( clip );
    m_currentUuid = uuid;
}

void
ImportController::newMediaLoaded( const QUuid& uuid )
{
    Media* media = Library::getInstance()->temporaryMedia( uuid );
    if ( media == NULL )
        return;
    m_mediaListController->addMedia( media );
}

void
ImportController::updateMediaRequested( const QUuid& uuid )
{
    Media* media = Library::getInstance()->temporaryMedia( uuid );
    if ( media == NULL )
        return;
    ImportMediaCellView*    cell = m_mediaListController->cell( media->getUuid() );
    if ( cell == NULL )
        return;
    cell->setThumbnail( media->getSnapshot() );
    cell->setLength( media->getLengthMS() );
}

void
ImportController::setUIMetaData( Media* media )
{
    if ( media != NULL )
    {
        //Duration
        QTime   duration;
        duration = duration.addMSecs( media->getLengthMS() );
        m_ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
        //Filename || title
        m_ui->nameValueLabel->setText( media->getFileInfo()->fileName() );
        m_ui->nameValueLabel->setWordWrap( true );
        setWindowTitle( media->getFileInfo()->fileName() + " " + tr( "properties" ) );
        //Resolution
        m_ui->resolutionValueLabel->setText( QString::number( media->getWidth() )
                                        + " x " + QString::number( media->getHeight() ) );
        //FPS
        m_ui->fpsValueLabel->setText( QString::number( media->getFps() ) );
    }
    else
    {
            m_ui->durationValueLabel->setText( "--:--:--" );
            m_ui->nameValueLabel->setText( "none" );
            m_ui->resolutionValueLabel->setText( "-- x --" );
            m_ui->fpsValueLabel->setText( "--" );
    }
}

void
ImportController::setUIMetaData( Clip* clip )
{
    QTime   time;
    qint64  length = clip->getLengthSecond();
    time = time.addSecs( length );
    m_ui->durationValueLabel->setText( time.toString( "hh:mm:ss" ) );
    m_ui->nameValueLabel->setText( clip->getParent()->getFileInfo()->fileName() );
    m_ui->nameValueLabel->setWordWrap( true );
    setWindowTitle( clip->getParent()->getFileInfo()->fileName() + " " +
                    tr( "properties" ) );
    m_ui->resolutionValueLabel->setText( QString::number( clip->getParent()->getWidth() )
            + " x " + QString::number( clip->getParent()->getHeight() ) );
    m_ui->fpsValueLabel->setText( QString::number( clip->getParent()->getFps() ) );
}

void
ImportController::forwardButtonClicked()
{
    QModelIndex index = m_ui->treeView->selectionModel()->currentIndex();
    QString filePath =  m_filesModel->fileInfo( index ).filePath();
    Library::getInstance()->loadFile( filePath );
}

void
ImportController::treeViewClicked( const QModelIndex& index )
{
    if ( m_filesModel->isDir( index ) )
    {
        m_fsWatcher->removePath( m_currentlyWatchedDir );
        m_currentlyWatchedDir = m_filesModel->filePath( index );
        m_fsWatcher->addPath( m_filesModel->filePath( index ) );
        saveCurrentPath();
    }
    m_ui->forwardButton->setEnabled( true );
}

void
ImportController::treeViewDoubleClicked( const QModelIndex& index )
{
    if ( !m_filesModel->isDir( index ) )
        forwardButtonClicked();
}

void
ImportController::reject()
{
    m_preview->stop();
    m_mediaListController->cleanAll();
    Library::getInstance()->deleteTemporaryMedias();
    collapseAllButCurrentPath();
    done( Rejected );
}

void
ImportController::accept()
{
    m_mediaListController->cleanAll();
    Library::getInstance()->importDone();
    m_preview->stop();
    collapseAllButCurrentPath();
    done( Accepted );
}

void
ImportController::collapseAllButCurrentPath()
{
    m_ui->treeView->collapseAll();
    QStringList paths;
    for ( QDir directory( m_currentlyWatchedDir ); !directory.isRoot(); directory.cdUp() )
        paths.prepend( directory.absolutePath() );
    while ( paths.count() > 0 )
    {
        m_ui->treeView->setCurrentIndex( m_filesModel->index( paths.takeFirst() ) );
        m_ui->treeView->setExpanded( m_ui->treeView->currentIndex() , true );
    }
    m_ui->forwardButton->setEnabled( true );
}

void
ImportController::mediaDeletion( const QUuid& uuid )
{
    Library::getInstance()->deleteMedia( uuid );
    m_mediaListController->removeMedia( uuid );

    if ( uuid == m_currentUuid )
    {
        setUIMetaData( static_cast<Media*>( 0 ) );
        m_currentUuid = QUuid();
        m_preview->stop();
    }
}

void
ImportController::clipDeletion( const QUuid& uuid )
{
    m_clipListController->removeClip( uuid );
    QUuid id;
    foreach( id, Library::getInstance()->temporaryMedias()->keys() )
    {
        Media* media = Library::getInstance()->temporaryMedias()->value( id );
        if ( media != NULL && media->clip( uuid ) )
            media->removeClip( uuid );
    }
}

void
ImportController::showClipList( const QUuid& uuid )
{
    Media* media = Library::getInstance()->temporaryMedia( uuid );
    if ( media == NULL || media->clips()->size() == 0 )
        return ;

    m_clipListController = new ImportMediaListController( m_stackNav );
    connect( m_clipListController, SIGNAL( clipSelected( const QUuid& ) ),
             this, SLOT( clipSelection( const QUuid& ) ) );
    connect( m_clipListController, SIGNAL( clipDeleted( const QUuid& ) ),
             this, SLOT( clipDeletion( const QUuid& ) ) );
    m_clipListController->addClipsFromMedia( media );
    if ( !m_currentUuid.isNull() )
        m_savedUuid = m_currentUuid;
    m_controllerSwitched = true;
    m_stackNav->pushViewController( m_clipListController );
}

void
ImportController::restoreContext()
{
    if ( m_clipListController->nbDeletions() != 0 )
    {
        if ( !m_savedUuid.isNull() && m_mediaListController->cell( m_savedUuid ) != NULL )
        {
            ImportMediaCellView* cell = m_mediaListController->cell( m_savedUuid );
            cell->decrementClipCount( m_clipListController->nbDeletions() );
        }
    }
    if ( !m_savedUuid.isNull() )
        m_currentUuid = m_savedUuid;
    m_controllerSwitched = false;
}

void
ImportController::saveCurrentPath()
{
    QSettings s;
    s.setValue( "ImportPreviouslySelectPath", m_currentlyWatchedDir );
    s.sync();
}

void
ImportController::restoreCurrentPath()
{
    QSettings s;
    QVariant path = s.value( "ImportPreviouslySelectPath", QDir::homePath() );
    QFileInfo info = path.toString();
    m_currentlyWatchedDir = info.absoluteFilePath();
}
