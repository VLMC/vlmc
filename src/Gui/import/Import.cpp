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

#include <QTime>

#include "Import.h"
#include "ui_Import.h"
#include "PreviewWidget.h"
#include "TagWidget.h"
#include "ImportMediaCellView.h"
#include "Media.h"
#include "ClipRenderer.h"
#include "Library.h"

Import::Import(QWidget *parent) :
    QDialog( parent ), m_ui( new Ui::Import )
{
    m_ui->setupUi( this );
    m_importBrowser = new ImportBrowser( m_ui->ImportBrowserWidget );
    m_previewWidget = new PreviewWidget( new ClipRenderer, m_ui->PreviewWidget );
    m_tagWidget = new TagWidget( m_ui->TagWidget, 6 );

    connect( m_importBrowser, SIGNAL( mediaAdded( Media*, ImportMediaCellView* ) ), this, SLOT( addMedia( Media*, ImportMediaCellView* ) ) );
    connect( m_importBrowser, SIGNAL( mediaRemoved() ), this, SLOT( removeMedia() ) );

    connect( this, SIGNAL( mediaSelected( Media* ) ), m_previewWidget->getGenericRenderer(), SLOT( setMedia( Media* ) ) );
    connect( this, SIGNAL( mediaSelected( Media* ) ), m_tagWidget, SLOT( mediaSelected( Media* ) ) );

}

Import::~Import()
{
    delete m_ui;
    delete m_importBrowser;
    delete m_previewWidget;
    delete m_tagWidget;
}

void    Import::addMedia( Media* media, ImportMediaCellView* cell )
{
    m_mediaList.insert( media->getUuid(), media );
    connect( cell, SIGNAL( mediaSelected( QUuid ) ), this, SLOT( setUIMetaData( QUuid ) ) );
    connect( cell->nextButton(), SIGNAL( clicked( QWidget*,QMouseEvent* ) ), this, SLOT( clipViewRequested( QWidget*,QMouseEvent* ) ) );
}

void    Import::removeMedia()
{
    m_mediaList.remove( m_currentUuid );
    m_importBrowser->getMediaListView()->removeMedia( m_currentUuid );
}

void    Import::setUIMetaData( QUuid uuid )
{
    m_currentUuid = uuid;

    //Duration
    QTime   duration;
    duration = duration.addSecs( m_mediaList[uuid]->getLength() );
    m_ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    //Filename || title
    m_ui->nameValueLabel->setText( m_mediaList[uuid]->getFileInfo()->fileName() );
    m_ui->nameValueLabel->setWordWrap( true );
    setWindowTitle( m_mediaList[uuid]->getFileInfo()->fileName() + " " + tr( "properties" ) );
    //Resolution
    m_ui->resolutionValueLabel->setText( QString::number( m_mediaList[uuid]->getWidth() )
                                       + " x " + QString::number( m_mediaList[uuid]->getHeight() ) );
    //FPS
    m_ui->fpsValueLabel->setText( QString::number( m_mediaList[uuid]->getFps() ) );

    emit mediaSelected( m_mediaList[uuid] );
}

void    Import::accept()
{
    QHashIterator<QUuid, Media*> iterator( m_mediaList );
    while ( iterator.hasNext() )
    {
        iterator.next();
        Library::getInstance()->addMedia( iterator.value() );
    }
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

void    Import::clipViewRequested( QWidget* sender, QMouseEvent* ev )
{
    MediaCellView* cell = qobject_cast<MediaCellView*>(sender->parent());
    ImportMediaListController* clipListView = m_importBrowser->getClipListView();

    if ( cell == NULL )
        return;
    clipListView->cleanAll();
    Media* media = m_mediaList[cell->uuid()];

    delete clipListView;
    clipListView = new ImportMediaListController(m_importBrowser->getStackViewController());

    m_importBrowser->getClipListView()->addClipsFromMedia( media );
    m_importBrowser->getStackViewController()->pushViewController( m_importBrowser->getClipListView() );
}
