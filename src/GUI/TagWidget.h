/*****************************************************************************
 * TagWidget.h : Widget for tagging media
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

#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QFileInfo>
#include <QCompleter>
#include "Clip.h"

namespace Ui
{
    class TagWidget;
}

class TagWidget : public QWidget
{
    Q_OBJECT
    public:
        TagWidget( QWidget *parent = 0, int nbButton = 0, QStringList tagList = QStringList() );
        ~TagWidget();
        void    setNbButton( int nbButton ) { m_nbButton = nbButton; }
        void    setTagList( QStringList tagList ) { m_defaultTagList = tagList; }

    protected:
        void    changeEvent( QEvent *e );
        void    setTagTextEdit();
        void    setButtonList( QStringList tagList );
        bool    isButtonExist();

    private:
        Ui::TagWidget*  m_ui;
        int             m_nbButton;
        QStringList     m_defaultTagList;
        QWidgetList     m_buttonList;
        Media*          m_currentMedia;

    public slots:
        void    mediaSelected( Media* media );
        void    buttonTagClicked();
        void    setMetaTags();
};

#endif // TAGWIDGET_H
