/*****************************************************************************
 * main.cpp: VLMC main
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <chavance.c@gmail.com>
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

#include "DockWidgetManager.h"

DockWidgetManager *DockWidgetManager::m_instance = 0;

DockWidgetManager *DockWidgetManager::instance()
{
    if (m_instance == 0)
        m_instance = new DockWidgetManager();

    return (m_instance);
}

void DockWidgetManager::setMainWindow( QMainWindow *mainWin )
{
    m_mainWin = mainWin;
}

void DockWidgetManager::addDockedWidget( QWidget *widget,
                                       const QString &qs_name,
                                       Qt::DockWidgetAreas areas,
                                       QDockWidget::DockWidgetFeature features,
                                       Qt::DockWidgetArea startArea)
{
    QDockWidget* dock = new QDockWidget( tr( qs_name.toStdString().c_str() ), m_mainWin );

    dock->setWidget( widget );
    dock->setAllowedAreas( areas );
    dock->setFeatures( features );
    m_mainWin->addDockWidget( startArea, dock );
}

DockWidgetManager::DockWidgetManager()
{
}
