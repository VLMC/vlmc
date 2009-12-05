/*****************************************************************************
 * Panel.h: a simple panel with button
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
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
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QString>
#include <QToolButton>

class   Panel : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Panel )
    public:
        Panel( QWidget* parent = 0 );
        ~Panel();

        void    addButton( const QString& label,
                           const QIcon& icon,
                           int number);
        void    show();

    private:
        QVBoxLayout*        m_layout;
        QButtonGroup*       m_buttons;
        QToolButton*        m_firstButton;
        int                 m_firstButtonNb;
        static const int    M_ICON_HEIGHT;

    private slots:
        void    switchPanel( int panel );

    signals:
        void   changePanel( int panel );
};


#endif /* !PANEL_H */
