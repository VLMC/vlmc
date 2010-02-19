/*****************************************************************************
 * ImportController.cpp
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
 *          Thomas Boquet <thomas.boquet@gmail.com>
 *          Clement Chavance <chavance.c@gmail.com>
 *          Hugo Beauzée-Luyssen <hugo@vlmc.org>
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
#include "MetaDataManager.h"

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
    m_filesModel = new QFileSystemModel( this );
    m_stackNav->pushViewController( m_mediaListController );

    QStringList filters;
    filters << Media::AudioExtensions.split(' ', QString::SkipEmptyParts)
            << Media::VideoExtensions.split(' ', QString::SkipEmptyParts)
            << Media::ImageExtensions.split(' ', QString::SkipEmptyParts);
    m_filesModel->setFilter( QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot );
    m_filesModel->sort( 2, Qt::AscendingOrder );
    m_filesModel->sort( 0, Qt::AscendingOrder );
    m_filesModel->setNameFilters( filters );
    m_filesModel->setRootPath( "/" );

    restoreCurrentPath();

    m_ui->treeView->setModel( m_filesModel );
    m_ui->treeView->setHeaderHidden( true );
    m_ui->treeView->setCurrentIndex( m_filesModel->index( m_currentlyWatchedDir ) );
    m_ui->treeView->setExpanded( m_ui->treeView->currentIndex() , true );
    m_ui->treeView->header()->setStretchLastSection( false );
    m_ui->treeView->header()->setResizeMode( QHeaderView::ResizeToContents );
    m_ui->treeView->setColumnHidden( 1, true );
    m_ui->treeView->setColumnHidden( 2, true );
    m_ui->treeView->setColumnHidden( 3, true );
    m_ui->forwardButton->setEnabled( true );

    m_progressDialog = new QProgressDialog( tr("Importing files..."),
                                            tr("Cancel"), 0, 0, NULL);
    m_progressDialog->setWindowModality( Qt::WindowModal );
    m_progressDialog->setMinimumDuration( 1000 );

    connect( m_ui->treeView, SIGNAL( clicked( QModelIndex ) ),
             this, SLOT( treeViewClicked( QModelIndex ) ) );
    connect( m_ui->treeView, SIGNAL( doubleClicked( QModelIndex ) ),
             this, SLOT( treeViewDoubleClicked( QModelIndex ) ) );
    connect( m_ui->forwardButton, SIGNAL( clicked() ),
             this, SLOT( forwardButtonClicked() ) );

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
    delete m_preview;
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

        setUIMetaData( m_temporaryMedias[uuid] );
        if ( uuid != NULL && uuid != m_currentUuid )
            m_preview->stop();
        m_currentUuid = uuid;
        emit mediaSelected( m_temporaryMedias[uuid] );
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
    foreach( Media* media, m_temporaryMedias.values() )
    {
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
ImportController::updateMediaRequested( Media *media )
{
    if ( m_temporaryMedias.contains( media->getUuid() ) == false )
        return ;
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
ImportController::importMedia( const QString &filePath )
{
    foreach ( Media* media, m_temporaryMedias.values() )
        if ( media->getFileInfo()->filePath() == filePath )
            return ;
    if ( Library::getInstance()->mediaAlreadyLoaded( filePath ) == true )
        return ;

    Media*          media = new Media( filePath );
    connect( media, SIGNAL( metaDataComputed( Media* ) ),
             this, SLOT( updateMediaRequested( Media* ) ) );
    connect( media, SIGNAL( snapshotComputed( Media* ) ),
             this, SLOT( updateMediaRequested( Media* ) ) );
    m_temporaryMedias[media->getUuid()] = media;
    MetaDataManager::getInstance()->computeMediaMetadata( media );
    m_mediaListController->addMedia( media );
}

void
ImportController::importDir( const QString &path )
{
    QDir            dir( path );
    QFileInfoList   files = dir.entryInfoList( QDir::NoDotAndDotDot | QDir::Readable
                                               | QDir::AllEntries );

    foreach ( QFileInfo fInfo, files )
    {
        if ( fInfo.isDir() == true )
            importDir( fInfo.absolutePath() );
        else
        {
            QString ext = fInfo.suffix();

            if ( Media::AudioExtensions.contains( ext ) ||
                 Media::VideoExtensions.contains( ext ) ||
                 Media::ImageExtensions.contains( ext ) )
            {
                importMedia( fInfo.absoluteFilePath() );
            }
        }
    }
}

void
ImportController::forwardButtonClicked()
{
    QModelIndex     index = m_ui->treeView->selectionModel()->currentIndex();
    QString         filePath = m_filesModel->fileInfo( index ).filePath();

    if ( !m_filesModel->isDir( index ) )
        importMedia( filePath );
    else
        importDir( filePath );
}

void
ImportController::treeViewClicked( const QModelIndex& index )
{
    if ( m_filesModel->isDir( index ) )
    {
        m_currentlyWatchedDir = m_filesModel->filePath( index );
        saveCurrentPath();
    }
    m_ui->forwardButton->setEnabled( true );
}

void
ImportController::treeViewDoubleClicked( const QModelIndex& index )
{
    if ( m_filesModel->isDir( index ) == false )
        forwardButtonClicked();
}

void
ImportController::reject()
{
    m_preview->stop();
    m_mediaListController->cleanAll();
    deleteTemporaryMedias();
    collapseAllButCurrentPath();
    done( Rejected );
}

void
ImportController::accept()
{
    m_mediaListController->cleanAll();
    m_preview->stop();
    collapseAllButCurrentPath();
    foreach ( Media* media, m_temporaryMedias.values() )
        Library::getInstance()->addMedia( media );
    m_temporaryMedias.clear();
    done( Accepted );
}

void
ImportController::deleteTemporaryMedias()
{
    foreach ( Media* media, m_temporaryMedias.values() )
        delete media;
    m_temporaryMedias.clear();
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
    m_mediaListController->removeMedia( uuid );
    if ( m_temporaryMedias.contains( uuid ) == true )
        delete m_temporaryMedias.take( uuid );

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
    foreach( Media* media, m_temporaryMedias.values() )
    {
        if ( media != NULL && media->clip( uuid ) )
            media->removeClip( uuid );
    }
}

void
ImportController::showClipList( const QUuid& uuid )
{
    if ( m_temporaryMedias.contains( uuid ) == false )
        return ;
    Media* media = m_temporaryMedias[uuid];
    if ( media->clips()->size() == 0 )
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

void
ImportController::progressDialogMax( int max )
{
    m_progressDialog->setMaximum( max );
}

void
ImportController::progressDialogValue( int value )
{
    m_progressDialog->setValue( value );
}
