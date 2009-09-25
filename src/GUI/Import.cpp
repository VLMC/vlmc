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

    m_ui->ImportBrowserWidget = m_importBrowser;
    m_ui->PreviewWidget = m_previewWidget;
    m_ui->TagWidget = m_tagWidget;

    connect( m_importBrowser, SIGNAL( mediaSelected( QFileInfo ) ), this, SLOT( getMetaData( QFileInfo ) ) );
}

Import::~Import()
{
    delete m_ui;
    delete m_currentMedia;
    delete m_currentClip;
    delete m_metaDataWorker;
    delete m_importBrowser;
    delete m_previewWidget;
    delete m_tagWidget;
}

void    Import::getMetaData( QFileInfo fileInfos )
{
    m_ui->nameValueLabel->setText( fileInfos.fileName() );
    m_currentMedia = new Media( fileInfos.filePath(), fileInfos.fileName() );

    m_metaDataWorker = new MetaDataWorker( m_currentMedia );
    connect( m_metaDataWorker, SIGNAL( destroyed() ), this, SLOT( setUIMetaData() ) );
    m_metaDataWorker->start();
}

void    Import::setUIMetaData()
{
    m_currentClip = new Clip( m_currentMedia );

    connect( this, SIGNAL( mediaSelected( Clip* ) ), m_previewWidget->getGenericRenderer(), SLOT( setClip( Clip* ) ) );
    connect( this, SIGNAL( mediaSelected( Media* ) ), m_tagWidget, SLOT( mediaSelected( Media* ) ) );
    emit mediaSelected( m_currentMedia );

    //Duration
    QTime   duration;
    duration = duration.addSecs( m_currentClip->getLengthSecond() );
    m_ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    //Filename || title
    m_ui->nameValueLabel->setText( m_currentClip->getParent()->getFileInfo()->fileName() );
    setWindowTitle( m_currentClip->getParent()->getFileInfo()->fileName() + " " + tr( "properties" ) );
    //Resolution
    m_ui->resolutionValueLabel->setText( QString::number( m_currentClip->getParent()->getWidth() )
                                       + " x " + QString::number( m_currentClip->getParent()->getHeight() ) );
    //FPS
    m_ui->fpsValueLabel->setText( QString::number( m_currentClip->getParent()->getFps() ) );

    emit mediaSelected( m_currentClip );

    disconnect( this, SIGNAL( mediaSelected( Clip* ) ), m_previewWidget->getGenericRenderer(), SLOT( setClip( Clip* ) ) );
}

void    Import::accept()
{
    //FIXME: When media are stored as Media*, replace newMediaLoadingAsked by addMedia
    for(int i = 0; i < m_importBrowser->getMediaInfoList().count(); i++ )
        Library::getInstance()->newMediaLoadingAsked( m_importBrowser->getMediaInfoList()[i].filePath() );
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
