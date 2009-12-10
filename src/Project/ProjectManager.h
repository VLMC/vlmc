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
#include <QStringList>

#include "Singleton.hpp"

class   ProjectManager : public QObject, public Singleton<ProjectManager>
{
    Q_OBJECT
    Q_DISABLE_COPY( ProjectManager );
public:
    void            loadProject( const QString& fileName );
    void            newProject( const QString& projectName );
    QString         loadProjectFile();
    void            saveProject( bool saveAs = true );
    bool            needSave() const;
    QStringList     recentsProjects() const;
    void            closeProject();
    bool            askForSaveIfModified();

private:
    void            parseProjectNode( const QDomElement& node );
    ProjectManager();
    ~ProjectManager();

    bool            checkProjectOpen( bool saveAs );

private:
    QFile*          m_projectFile;
    QDomDocument*   m_domDocument;
    bool            m_needSave;
    QStringList     m_recentsProjects;
    QString         m_projectName;

    friend class    Singleton<ProjectManager>;

private slots:
    void            loadTimeline();
    void            cleanChanged( bool val );
    void            nameChanged( const QVariant& name );

signals:
    /**
     *  This signal is emitted when :
     *      - The project name has changed
     *      - The clean state has changed
     *      - The revision (if activated) has changed
     */
    void            projectUpdated( const QString& projectName, bool savedState );
    void            projectSaved();
    void            projectClosed();
};

#endif // PROJECTMANAGER_H
