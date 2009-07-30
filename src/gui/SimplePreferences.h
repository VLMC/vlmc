/*****************************************************************************
 * SimplePreferences.h: generic preferences interface
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

#ifndef SIMPLEPREFERENCES_H
#define SIMPLEPREFERENCES_H

#include <QWidget>
#include <QString>
#include <QHash>
#include <QLabel>
#include <QVBoxLayout>
#include <QButtonGroup>

#include "Panel.h"

class   SimplePreferences : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( SimplePreferences )

    public:
        SimplePreferences( QWidget* parent = 0 );
        ~SimplePreferences();
        void                addWidget( const QString& name,
                                       QWidget* widget,
                                       const QString& icon,
                                       const QString& label );
        void                build();

    private:
        QVBoxLayout*    buildRightHLayout();

    private:
        QHash<int, QWidget*>        m_widgets;
        QWidget*                    m_currentWidget;
        Panel*                      m_panel;
        QLabel*                     m_title;
        int                         m_widgetNumber;

    public slots:
        void    switchWidget( int widget );
};

#endif /* !SIMPLEPREFERENCES_H */
