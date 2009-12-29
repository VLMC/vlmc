/*****************************************************************************
 * DockWidgetManager.h: Object managing the application docked widget
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

#ifndef DOCKWIDGETMANAGER_H
#define DOCKWIDGETMANAGER_H

#include <QObject>
#include <QWidget>
#include <QDockWidget>
#include <QString>
#include <QMap>

class MainWindow;

class DockWidgetManager : public QObject
{
    Q_OBJECT

    public:
        static DockWidgetManager *instance( QObject *parent = 0 );
        void setMainWindow( MainWindow *mainWin );
        void addDockedWidget( QWidget *widget,
                              const QString &qs_name,
                              Qt::DockWidgetAreas areas,
                              QDockWidget::DockWidgetFeature features,
                              Qt::DockWidgetArea startArea );

    protected:
        //virtual void changeEvent( QEvent *e );

    private:
        explicit DockWidgetManager( QObject *parent = 0 );
        virtual ~DockWidgetManager();
        DockWidgetManager(const DockWidgetManager &);
        DockWidgetManager & operator = ( const DockWidgetManager & );

        MainWindow *m_mainWin;
        QMap<QString, QDockWidget*> m_dockWidgets;
        static DockWidgetManager *m_instance;

    public slots:
        void transLateWidgetTitle();
};

#endif
