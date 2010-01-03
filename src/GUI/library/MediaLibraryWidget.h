/*****************************************************************************
 * MediaLibraryWidget.h
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Thomas Boquet <thomas.boquet@gmail.com>
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

#ifndef MEDIALIBRARYWIDGET_H
#define MEDIALIBRARYWIDGET_H

#include <QWidget>

class StackViewController;
class ViewController;
class Media;
class Clip;

class MediaLibraryWidget : public QWidget
{
    Q_OBJECT

public:
    MediaLibraryWidget( QWidget* parent = 0 );
    virtual ~MediaLibraryWidget();

    const ViewController*   getCurrentViewController();

private:
    StackViewController*    m_nav;

public slots:
    void    clipAdded( Clip* clip );
private slots:
    void    mediaSelection( Media* media );
    void    clipSelection( Clip* clip );

signals:
    void    mediaSelected( Media* media );
    void    clipSelected( Clip* clip );
    void    importRequired();
    void    addClipToViewController( Clip* clip );
};

#endif // MEDIALIBRARYWIDGET_H
