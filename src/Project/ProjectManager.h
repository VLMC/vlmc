/*****************************************************************************
 * ProjectManager.h: Manager the project loading and saving.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QFile>
#include <QObject>
#include <QDomDocument>

class   ProjectManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( ProjectManager );
public:
    ProjectManager( const QString& filePath );
    ~ProjectManager();

    void        loadProject();
    void        saveProject();

private:
    QFile*          m_projectFile;
    QDomDocument*   m_domDocument;


private slots:
    void        loadTimeline();
};

#endif // PROJECTMANAGER_H
