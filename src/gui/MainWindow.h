/*****************************************************************************
 * MainWindow.h: VLMC MainWindow
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include "ui_MainWindow.h"
#include "LibraryWidget.h"
#include "PreviewWidget.h"
#include "DockWidgetManager.h"
#include "Preferences.h"
#include "MetaDataManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY( MainWindow )

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

protected:
    virtual void changeEvent( QEvent *e );

private:
    void m_initializeDockWidgets( void );


    Ui::MainWindow      m_ui;
    LibraryWidget*      m_library;
    MetaDataManager*    m_metaDataManager;

private slots:
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionPreferences_triggered();
    void on_actionTranscode_File_triggered();

signals:
    void translateDockWidgetTitle();

};

#endif // MAINWINDOW_H
