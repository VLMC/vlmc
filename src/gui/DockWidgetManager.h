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

#ifndef DOCKWIDGETMANAGER_H
#define DOCKWIDGETMANAGER_H

#include <QObject>
#include <QMainWindow>
#include <QWidget>
#include <QDockWidget>
#include <QString>

class DockWidgetManager : public QObject
{
	Q_OBJECT

	public:
		explicit DockWidgetManager( QMainWindow *mainWin );
		void addDockedWidget( QWidget *widget,
							  const QString &qs_name,
							  Qt::DockWidgetAreas areas,
							  QDockWidget::DockWidgetFeature features,
							  Qt::DockWidgetArea startArea );

	private:
		QMainWindow *m_mainWin;
};

#endif
