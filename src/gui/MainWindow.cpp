/*****************************************************************************
 * MainWindow.cpp: VLMC MainWindow
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#include <QLabel>
#include <QSizePolicy>
#include <QPalette>
#include <QDockWidget>
#include "MainWindow.h"
#include "Timeline.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent )
{
    m_ui.setupUi( this );
    DockWidgetManager::instance()->setMainWindow( this );
    m_initializeDockWidgets();
}

void MainWindow::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        break;
    }
}

void MainWindow::m_initializeDockWidgets( void )
{
    Timeline* timeline = new Timeline( this );
    timeline->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    timeline->show();
    setCentralWidget( timeline );

    DockWidgetManager *dockManager = DockWidgetManager::instance();

    //First param is NULL for the moment. It will be replaced by the LibraryWidget
    dockManager->addDockedWidget( new LibraryWidget( this ),
                                  tr( "Media Library" ),
                                  Qt::AllDockWidgetAreas, 
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );
    //dockManager->addDockedWidget( 0,
    //                              "library2",
    //                              Qt::AllDockWidgetAreas,
    //                              QDockWidget::AllDockWidgetFeatures,
    //                              Qt::RightDockWidgetArea );

    //Create the library widget
}

//Private slots definition

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
