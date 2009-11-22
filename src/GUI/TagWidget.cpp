/*****************************************************************************
 * TagWidget.cpp : Widget for tagging media
 *                     Render preview
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

#include "TagWidget.h"
#include "ui_TagWidget.h"
#include <QPushButton>
#include <QDebug>
#include <math.h>

TagWidget::TagWidget( QWidget *parent, int nbButton, QStringList tagList ) :
    QWidget( parent ), m_ui( new Ui::TagWidget ), m_nbButton( nbButton ), m_defaultTagList( tagList ), m_currentMedia( NULL )
{
    m_ui->setupUi( this );
    m_defaultTagList << "OutDoor" << "Hollidays" << "Seaside" << "Sunset" << "Familly";

//    int nbRow =  sqrt( nbButton );
//    int x = 0;
//    int y = 0;
//
//    for( int i = 0; i < nbButton; i++ )
//    {
//        if ( x == nbRow )
//        {
//            x = 0;
//            y++;
//        }
//        QPushButton* button = new QPushButton();
//        button->setCheckable( true );
//        connect( button, SIGNAL( clicked() ), this, SLOT( buttonTagClicked() ) );
//        m_buttonList.append( button );
//        m_ui->buttonGridLayout->addWidget( button, x++, y );
//    }

    for( int i = 0; i < m_defaultTagList.count(); i++ )
        if ( i < m_buttonList.count() )
        {
            static_cast<QPushButton*>( m_buttonList[i])->setText( m_defaultTagList[i] );
            static_cast<QPushButton*>( m_buttonList[i])->setEnabled( false );
        }

    connect( m_ui->TagTextEdit, SIGNAL( textChanged() ), this, SLOT( setMetaTags() ) );
}

TagWidget::~TagWidget()
{
    delete m_ui;
}

void    TagWidget::mediaSelected( Media* media )
{
    m_currentMedia = media;
    setTagTextEdit();
    for (int i = 0; i < m_buttonList.count(); i++)
    {
        static_cast<QPushButton*>( m_buttonList[i])->setEnabled( true );
        if ( m_currentMedia->getMetaTags().contains( static_cast<QPushButton*>(m_buttonList[i])->text() ) )
            static_cast<QPushButton*>(m_buttonList[i])->setChecked( true );
        else
            static_cast<QPushButton*>(m_buttonList[i])->setChecked( false );
    }
}

void    TagWidget::setMetaTags()
{
    if ( m_currentMedia != NULL )
    {
        QStringList tagList = m_ui->TagTextEdit->document()->toPlainText().split( ",", QString::SkipEmptyParts );
        m_currentMedia->setMetaTags( tagList );
    }
}

void    TagWidget:: buttonTagClicked()
{
    if ( m_currentMedia != NULL )
    {
        QStringList tagList = m_currentMedia->getMetaTags();
        for (int i = 0; i < m_buttonList.count(); i++)
        {
            QPushButton* button = static_cast<QPushButton*>(m_buttonList[i]);
            if ( button->isChecked() && !tagList.contains( button->text() ) )
                tagList << button->text();
            else if ( !button->isChecked() && tagList.contains( button->text() ) )
                tagList.removeAll( button->text() );
        }
        m_currentMedia->setMetaTags( tagList );
        setTagTextEdit();
    }
}

void    TagWidget::setTagTextEdit()
{
    QString tags;
    if ( m_currentMedia != NULL )
    {
        for( int i = 0; i < m_currentMedia->getMetaTags().count(); i++ )
            if (i == 0)
                tags += m_currentMedia->getMetaTags()[i];
            else
                tags += "," + m_currentMedia->getMetaTags()[i];
        m_ui->TagTextEdit->setText(tags);
        setButtonList( m_defaultTagList );
    }
}

void    TagWidget::setButtonList( QStringList tagList )
{
    if ( m_currentMedia != NULL )
        for( int i = 0; i < tagList.count(); i++ )
            if ( i < m_buttonList.count() )
                static_cast<QPushButton*>( m_buttonList[i] )->setText( tagList[i] );
}

void TagWidget::changeEvent( QEvent *e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
        case QEvent::LanguageChange:
            m_ui->retranslateUi( this );
            break;
        default:
            break;
    }
}
