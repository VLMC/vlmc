/*****************************************************************************
 * Import.cpp: Import menu
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

#include "Import.h"
#include "ui_Import.h"
#include "ClipRenderer.h"
#include "Library.h"
#include "QTime"

Import::Import(QWidget *parent) :
    QDialog( parent ), m_ui( new Ui::Import )
{
    m_ui->setupUi( this );
    m_importBrowser = new ImportBrowser( m_ui->ImportBrowserWidget );
    m_previewWidget = new PreviewWidget( new ClipRenderer, m_ui->PreviewWidget );
    m_tagWidget = new TagWidget( m_ui->TagWidget, 6 );

    connect( m_importBrowser, SIGNAL( mediaAdded( QFileInfo ) ), this, SLOT( addMedia( QFileInfo ) ) );
    connect( m_importBrowser, SIGNAL( mediaRemoved( QFileInfo ) ), this, SLOT( removeMedia( QFileInfo ) ) );
    connect( m_importBrowser, SIGNAL( mediaSelected( QFileInfo ) ), this, SLOT( selectCurrentMedia( QFileInfo ) ) );

    connect( this, SIGNAL( mediaSelected( Media* ) ), m_previewWidget->getGenericRenderer(), SLOT( setMedia( Media* ) ) );
    connect( this, SIGNAL( mediaSelected( Media* ) ), m_tagWidget, SLOT( mediaSelected( Media* ) ) );
}

Import::~Import()
{
    delete m_ui;
    delete m_currentMedia;
    delete m_metaDataWorker;
    delete m_importBrowser;
    delete m_previewWidget;
    delete m_tagWidget;
}

void    Import::addMedia( QFileInfo fileInfos )
{
    m_ui->nameValueLabel->setText( fileInfos.fileName() );
    m_currentMedia = new Media( fileInfos.filePath() );
    m_mediaList.append( m_currentMedia );

    m_metaDataWorker = new MetaDataWorker( m_currentMedia );
    m_metaDataWorker->start();
}

void    Import::removeMedia( QFileInfo fileInfos )
{
    for ( int i = 0; i < m_mediaList.count(); i++ )
    {
        if ( fileInfos == *m_mediaList[i]->getFileInfo() )
        {
            m_mediaList.removeAt( i );
            m_currentMedia = NULL;
            break;
        }
    }
}

void    Import::selectCurrentMedia( QFileInfo fileInfos )
{
    for ( int i = 0; i < m_mediaList.count(); i++ )
    {
        if ( fileInfos == *m_mediaList[i]->getFileInfo() )
        {
            m_currentMedia = m_mediaList[i];
            // TODO: stop the media player to stop before sending the mediaSelected signal
            emit mediaSelected( m_currentMedia );
            setUIMetaData();
            break;
        }
    }
}

void    Import::setUIMetaData()
{
    //Duration
    QTime   duration;
    duration = duration.addSecs( m_currentMedia->getLength() );
    m_ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    //Filename || title
    m_ui->nameValueLabel->setText( m_currentMedia->getFileInfo()->fileName() );
    setWindowTitle( m_currentMedia->getFileInfo()->fileName() + " " + tr( "properties" ) );
    //Resolution
    m_ui->resolutionValueLabel->setText( QString::number( m_currentMedia->getWidth() )
                                       + " x " + QString::number( m_currentMedia->getHeight() ) );
    //FPS
    m_ui->fpsValueLabel->setText( QString::number( m_currentMedia->getFps() ) );
}

void    Import::accept()
{
    for ( int i = 0; i < m_mediaList.count(); i++ )
        Library::getInstance()->addMedia( m_mediaList[i] );
    done(Accepted);
}

void Import::changeEvent( QEvent *e )
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
