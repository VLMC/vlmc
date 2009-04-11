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
#include "About.h"
#include "Transcode.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent )
{
    m_ui.setupUi( this );
    DockWidgetManager::instance( this )->setMainWindow( this );
    m_initializeDockWidgets();

    QObject::connect( this,
                      SIGNAL( translateDockWidgetTitle() ),
                      DockWidgetManager::instance(),
                      SLOT(transLateWidgetTitle() ) );
}

MainWindow::~MainWindow()
{
    if (m_metaDataManager)
        delete m_metaDataManager;
}

void MainWindow::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        emit translateDockWidgetTitle();
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

    LibraryWidget* libraryWidget = LibraryWidget::getInstance( this );
    dockManager->addDockedWidget( libraryWidget,
                                  tr( "Media Library" ),
                                  Qt::AllDockWidgetAreas, 
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );

    dockManager->addDockedWidget( new PreviewWidget( this ),
                                  tr( "Preview" ),
                                  Qt::AllDockWidgetAreas,
                                  QDockWidget::AllDockWidgetFeatures,
                                  Qt::TopDockWidgetArea );

    m_metaDataManager = new MetaDataManager( libraryWidget );
}

//Private slots definition

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionPreferences_triggered()
{
    Preferences::instance()->exec();
}

void MainWindow::on_actionAbout_triggered()
{
    About::instance()->exec();
}

void MainWindow::on_actionTranscode_File_triggered()
{
    Transcode::instance( this )->exec();
}
