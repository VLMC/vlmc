/*****************************************************************************
 * WelcomePage.cpp: Wizard welcome page
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
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include "WelcomePage.h"

QString* WelcomePage::m_projectPath = NULL;

WelcomePage::WelcomePage( QWidget* parent )
    : QWizardPage( parent )
{
    m_ui.setupUi( this );

    setTitle( tr( "Project wizard" ) );
    setSubTitle( tr( "Open or create a project" ) );

    connect( m_ui.openPushButton, SIGNAL( clicked() ),
             this, SLOT( loadProject() ) );
    connect( m_ui.projectsListWidget, SIGNAL( itemPressed(QListWidgetItem*) ),
             this, SLOT( selectOpenRadio() ) );
    connect( m_ui.projectsListWidget, SIGNAL( itemDoubleClicked(QListWidgetItem*) ),
             this, SLOT( projectDoubleClicked(QListWidgetItem*) ) );

    registerField( "loadProject", m_ui.projectsListWidget );
    m_projectPath = new QString();
}

WelcomePage::~WelcomePage()
{
    delete m_projectPath;
}

void WelcomePage::changeEvent( QEvent *e )
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

int WelcomePage::nextId() const
{
    if ( m_ui.createRadioButton->isChecked() )
        return ProjectWizard::Page_General;
    else
        return ProjectWizard::Page_Open;
}

void WelcomePage::initializePage()
{
    m_ui.createRadioButton->setChecked( true );
    loadRecentsProjects();
}

bool WelcomePage::validatePage()
{
    if ( m_ui.openRadioButton->isChecked() )
    {
        if ( m_ui.projectsListWidget->selectedItems().count() == 0 )
        {
            QMessageBox::information( this, tr( "Sorry" ),
                                      tr( "You first need to select a project from "
                                      "the list.\nThen click next to continue..." ) );
            return false;
        }
        QList<QListWidgetItem*> selected = m_ui.projectsListWidget->selectedItems();
        setProjectPath( selected.at( 0 )->data( FilePath ).toString() );
        return true;
    }
    return true;
}

void WelcomePage::cleanupPage()
{

}

void WelcomePage::loadRecentsProjects()
{
    m_ui.projectsListWidget->clear();
    ProjectManager* pm = ProjectManager::getInstance();
    QStringList recents = pm->recentsProjects();

    for ( int i = 0; i < recents.count(); ++i )
    {
        QFileInfo fi( recents.at( i ) );
        QListWidgetItem* item = new QListWidgetItem( fi.fileName() );
        item->setData( FilePath, fi.absoluteFilePath() );

        m_ui.projectsListWidget->addItem( item );
    }
}

void WelcomePage::loadProject()
{
    ProjectManager* pm = ProjectManager::getInstance();
    QString projectPath = pm->loadProjectFile();

    if ( projectPath.isEmpty() ) return;

    // Search if the item is already in the list
    QListWidgetItem* item = NULL;
    for ( int i = 0; i < m_ui.projectsListWidget->count(); ++i )
    {
        item = m_ui.projectsListWidget->item( i );
        if ( item->data( FilePath ).toString().contains( projectPath ) )
            break;
        item = NULL;
    }

    // Item not in list, insert it temporarily
    if ( !item )
    {
        QFileInfo fi( projectPath );
        item = new QListWidgetItem( fi.fileName() );
        item->setData( FilePath, fi.absoluteFilePath() );

        m_ui.projectsListWidget->addItem( item );
    }

    item->setSelected( true );
    selectOpenRadio();
}

void WelcomePage::selectOpenRadio()
{
    m_ui.openRadioButton->setChecked( true );
}

void WelcomePage::projectDoubleClicked( QListWidgetItem* item )
{
    Q_UNUSED( item );
    if ( wizard() )
        wizard()->next();
}

QString WelcomePage::projectPath()
{
    return *m_projectPath;
}

void WelcomePage::setProjectPath( const QString& path )
{
    m_projectPath->clear();
    m_projectPath->append( path );
}
