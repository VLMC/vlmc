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
    QWidget( parent ), m_ui( new Ui::TagWidget ), m_nbButton( nbButton ), m_defaultTagList( tagList )
{
    m_ui->setupUi( this );
    m_defaultTagList << "Ta chatte" << "Maman" << "Chiwawa" << "Bite" << "Porcinet";

    int nbRow =  sqrt( nbButton );
    int x = 0;
    int y = 0;

    for( int i = 0; i < nbButton; i++ )
    {
        if ( x == nbRow )
        {
            x = 0;
            y++;
        }
        QPushButton* button = new QPushButton();
        m_buttonList.append( button );
        m_ui->buttonGridLayout->addWidget( button, x++, y );
    }

    for( int i = 0; i < m_defaultTagList.count(); i++ )
        if ( i < m_buttonList.count() )
            static_cast<QPushButton*>( m_buttonList[i])->setText( m_defaultTagList[i] );

    setTagTextEdit();

    m_completer = new QCompleter( m_defaultTagList, this );
    m_completer->setCaseSensitivity( Qt::CaseInsensitive );
    m_ui->KeywordTagLineEdit->setCompleter( m_completer );

    connect( m_ui->AddButton, SIGNAL( clicked() ), this, SLOT( tagAdded() ) );
}

TagWidget::~TagWidget()
{
    delete m_ui;
}

void    TagWidget::setTagTextEdit()
{
    QString tags;
    for( int i = 0; i < m_defaultTagList.count(); i++ )
        tags += " " + m_defaultTagList[i];
    m_ui->TagTextEdit->setText(tags);
}

void    TagWidget::tagAdded()
{
    m_defaultTagList << m_ui->KeywordTagLineEdit->text();
    m_ui->KeywordTagLineEdit->clear();
    setTagTextEdit();
}

void    TagWidget::mediaSelected( Media* media )
{
    m_currentMedia = media;
    setTagTextEdit();
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
