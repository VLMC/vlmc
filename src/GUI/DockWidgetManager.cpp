/*****************************************************************************
 * DockWidgetManager.cpp: Object managing the application docked widget
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
#include <QtDebug>
#include <QApplication>
#include <QMap>
#include <QDockWidget>

#include "DockWidgetManager.h"
#include "MainWindow.h"

DockWidgetManager *DockWidgetManager::m_instance = 0;

DockWidgetManager *DockWidgetManager::instance( QObject *parent )
{
    if (m_instance == 0)
        m_instance = new DockWidgetManager( parent );

    return (m_instance);
}

void DockWidgetManager::setMainWindow( MainWindow *mainWin )
{
    m_mainWin = mainWin;
}

void DockWidgetManager::addDockedWidget( QWidget *widget,
                                       const QString &qs_name,
                                       Qt::DockWidgetAreas areas,
                                       QDockWidget::DockWidgetFeature features,
                                       Qt::DockWidgetArea startArea)
{
    if ( m_dockWidgets.contains( qs_name ) )
        return ;

    QDockWidget* dock = new QDockWidget( tr( qs_name.toStdString().c_str() ), m_mainWin );

    m_dockWidgets.insert(qs_name, dock);
    dock->setWidget( widget );
    dock->setAllowedAreas( areas );
    dock->setFeatures( features );
    dock->setObjectName( qs_name );
    m_mainWin->addDockWidget( startArea, dock );
    m_mainWin->registerWidgetInWindowMenu( dock );
    widget->show();
}


DockWidgetManager::DockWidgetManager( QObject *parent )
    : QObject( parent )
{
    QObject::connect( qApp,
                      SIGNAL( aboutToQuit() ),
                      this,
                      SLOT( deleteLater() ) );
}

DockWidgetManager::~DockWidgetManager()
{
    QList<QDockWidget*> widgets = m_dockWidgets.values();
    QDockWidget* widget;

    foreach(widget, widgets)
    {
        delete widget;
    }
}

void DockWidgetManager::transLateWidgetTitle()
{
    QMap<QString, QDockWidget*>::iterator ed = m_dockWidgets.end();
    QMap<QString, QDockWidget*>::iterator it;

    for ( it = m_dockWidgets.begin() ; it != ed ; ++it )
    {
        QDockWidget *widget = it.value();
        widget->setWindowTitle( QApplication::translate( "MainWindow",
                                                         it.key().toStdString().c_str() ) );
    }
}
