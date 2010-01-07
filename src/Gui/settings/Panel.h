/*****************************************************************************
 * Panel.h: a simple panel with buttons
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <QString>

class QVBoxLayout;
class QButtonGroup;

class QButtonGroup;

class   Panel : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Panel )

public:
    Panel( QWidget* parent = 0 );
    virtual ~Panel() { }

    void    addButton( const QString& label,
                       const QIcon& icon,
                       int index );

private:
    virtual void        showEvent( QShowEvent * );
    QVBoxLayout*        m_layout;
    QButtonGroup*       m_buttons;
    static const int    M_ICON_HEIGHT;

signals:
    void   changePanel( int index );
};


#endif /* !PANEL_H */
