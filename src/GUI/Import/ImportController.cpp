/*****************************************************************************
 * ImportController.cpp
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
 *          Thomas Boquet <thomas.boquet@gmail.com>
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

#include <QTime>
#include "ImportController.h"
#include "ui_ImportController.h"
#include "ClipRenderer.h"
#include "Library.h"
#include "QPalette"

ImportController::ImportController(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ImportController)
{
    m_ui->setupUi(this);

    m_model = new ImportModel();
    m_preview = new PreviewWidget( new ClipRenderer, m_ui->previewContainer );
    m_stackNav = new StackViewController( m_ui->stackViewContainer, false );
    m_mediaListController = new ImportMediaListController( m_stackNav );
    m_tag = new TagWidget( m_ui->tagContainer, 6 );
    m_filesModel = new QDirModel();

    m_stackNav->pushViewController( m_mediaListController );

    QStringList filters;
    //Video
    filters << "*.mov" << "*.avi" << "*.mkv" << "*.mpg" << "*.mpeg" << "*.wmv" << "*.mp4";
    //Audio
    filters << "*.mp3" << "*.oga" << "*.flac" << "*.aac" << "*.wav";
    //Picture
    filters << "*.gif" << "*.png" << "*.jpg";

    m_filesModel->setFilter( QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot );
    m_filesModel->sort( 2, Qt::AscendingOrder );
    m_filesModel->sort( 0, Qt::AscendingOrder );
    m_filesModel->setNameFilters( filters );

    m_ui->treeView->setModel( m_filesModel );
    m_ui->treeView->setRootIndex( m_filesModel->index( QDir::rootPath() ) );
    m_ui->treeView->setCurrentIndex( m_filesModel->index( QDir::homePath() ) );
    m_ui->treeView->setExpanded( m_ui->treeView->currentIndex() , true );
    m_ui->treeView->setColumnHidden( 1, true );
    m_ui->treeView->setColumnHidden( 2, true );
    m_ui->treeView->setColumnHidden( 3, true );
    m_ui->forwardButton->setEnabled( false );

    m_fsWatcher = new QFileSystemWatcher();
    m_fsWatcher->addPath( QDir::homePath() );
    m_currentlyWatchedDir = QDir::homePath();

    connect( m_fsWatcher, SIGNAL( directoryChanged( QString ) ), m_filesModel, SLOT( refresh() ) );
    connect( m_ui->treeView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( treeViewClicked( QModelIndex ) ) );
    connect( m_ui->treeView, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( treeViewDoubleClicked( QModelIndex ) ) );
    connect( m_ui->forwardButton, SIGNAL( clicked() ), this, SLOT( forwardButtonClicked() ) );

    connect( m_model, SIGNAL( newMediaLoaded( Media* ) ), this, SLOT( newMediaLoaded( Media* ) ) );
    connect( m_model, SIGNAL( updateMediaRequested( Media* ) ), this, SLOT( updateMediaRequested( Media* ) ) );

    connect( this, SIGNAL( mediaSelected( Media* ) ), m_preview->getGenericRenderer(), SLOT( setMedia( Media* ) ) );
    connect( this, SIGNAL( mediaSelected( Media* ) ), m_tag, SLOT( mediaSelected( Media* ) ) );

    connect( m_mediaListController, SIGNAL( mediaSelected( QUuid ) ), this, SLOT( mediaSelection( QUuid ) ) );
    connect( m_mediaListController, SIGNAL( mediaDeleted( QUuid ) ), this, SLOT( mediaDeletion( QUuid ) ) );
}

ImportController::~ImportController()
{
    delete m_ui;
    delete m_model;
    delete m_stackNav;
    delete m_tag;
}

void ImportController::changeEvent( QEvent *e )
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

void        ImportController::newMediaLoaded( Media* media )
{
    m_mediaListController->addMedia( media );
}

void        ImportController::mediaSelection( const QUuid& uuid )
{
    if ( !m_currentUuid.isNull() )
        m_mediaListController->getCell( m_currentUuid )->setPalette( palette() );
    QPalette p = m_mediaListController->getCell( uuid )->palette();
    p.setColor( QPalette::Window, QColor( Qt::darkBlue ) );
    m_mediaListController->getCell( uuid )->setPalette( p );

    setUIMetaData( m_model->getMedia( uuid ) );
    emit mediaSelected( m_model->getMedia( uuid ) );
    m_currentUuid = uuid;
}

void        ImportController::clipSelection( const QUuid& uuid )
{
    Q_UNUSED( uuid );
}

void        ImportController::updateMediaRequested( Media* media )
{
    m_mediaListController->getCell( media->getUuid() )->setThumbnail( media->getSnapshot() );
}
void    ImportController::setUIMetaData( Media* media )
{
    if ( media != NULL )
    {
        //Duration
        QTime   duration;
        duration = duration.addSecs( media->getLengthMS() );
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


void    ImportController::forwardButtonClicked()
{
    m_model->loadFile( m_filesModel->fileInfo( m_ui->treeView->selectionModel()->currentIndex() ).filePath() );
}

void    ImportController::treeViewClicked( const QModelIndex& index )
{
    if ( m_filesModel->isDir( index ) )
    {
        m_fsWatcher->removePath( m_currentlyWatchedDir );
        m_currentlyWatchedDir = m_filesModel->filePath( index );
        m_fsWatcher->addPath( m_filesModel->filePath( index ) );
    }
    m_ui->forwardButton->setEnabled( true );
}

void    ImportController::treeViewDoubleClicked( const QModelIndex& index )
{
    if ( !m_filesModel->isDir( index ) )
        forwardButtonClicked();
}

void    ImportController::reject()
{
    m_preview->stop();
}

void    ImportController::accept()
{
    QUuid id;
    foreach( id, m_model->getMedias()->keys() )
    {
        Media* media = m_model->getMedias()->value( id );
        Library::getInstance()->addMedia( media );
    }
    m_preview->stop();
    done( Accepted );
}

void        ImportController::mediaDeletion( const QUuid& uuid )
{
    m_mediaListController->removeMedia( uuid );
    delete m_model->getMedias()->take( uuid );
    if ( uuid == m_currentUuid )
    {
        setUIMetaData( NULL );
        m_currentUuid = QUuid();
        m_preview->stop();
    }
}

void        ImportController::clipDeletion( const QUuid& uuid )
{
    m_mediaListController->removeClip( uuid );
    QUuid id;
    foreach(id, m_model->getMedias()->keys() )
    {
        Media* media = m_model->getMedias()->value( id );
        if (media->clip( uuid) )
            media->removeClip( uuid );
    }
}
