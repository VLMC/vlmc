#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include "GeneralPage.h"
#include "ProjectWizard.h"
#include "SettingsManager.h"

GeneralPage::GeneralPage( QWidget *parent ) :
    QWizardPage( parent )
{
    ui.setupUi( this );

    setTitle( tr( "New project wizard" ) );
    setSubTitle( tr( "Set General options" ) );

    // Create palettes
    pValid = pInvalid = palette();
    pInvalid.setColor( QPalette::Text, QColor( 215, 30, 30 ) );

    connect( ui.pushButtonBrowse, SIGNAL( clicked() ),
             this, SLOT( openWorkspaceDirectory() ) );
    connect( ui.lineEditName, SIGNAL( textChanged(QString) ),
             this, SLOT( updateProjectLocation() ) );
    connect( ui.lineEditWorkspace, SIGNAL( textChanged(QString) ),
             this, SLOT( updateProjectLocation() ) );

    registerField( "projectName*", ui.lineEditName );
}

void GeneralPage::changeEvent( QEvent *e )
{
    QWizardPage::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        ui.retranslateUi( this );
        break;
    default:
        break;
    }
}

int GeneralPage::nextId() const
{
    return ProjectWizard::Page_Video;
}

void GeneralPage::initializePage()
{
    SettingsManager* sManager = SettingsManager::getInstance();

    QString projectName = sManager->getValue( "default", "ProjectName" )->get().toString();
    ui.lineEditName->setText( projectName );

    QString workspacePath = sManager->getValue( "default", "VLMCWorkspace" )->get().toString();
    ui.lineEditWorkspace->setText( workspacePath );

    updateProjectLocation();
}

void GeneralPage::cleanupPage()
{

}

bool GeneralPage::validatePage()
{
    SettingsManager* sManager = SettingsManager::getInstance();

    QString defaultProjectName = sManager->getValue( "default", "ProjectName" )->get().toString();
    if ( ui.lineEditName->text().isEmpty() || ui.lineEditName->text() == defaultProjectName )
    {
        QMessageBox::information( this, tr( "Form is incomplete" ),
                                  tr( "The project name must be filled." ) );
        ui.lineEditName->setFocus();
        return false;
    }
    if ( ui.lineEditWorkspace->text().isEmpty() )
    {
        QMessageBox::information( this, tr( "Form is incomplete" ),
                                  tr( "The workspace location must be set." ) );
        ui.lineEditWorkspace->setFocus();
        return false;
    }

    QVariant projectName( ui.lineEditName->text() );
    sManager->setValue( "project", "ProjectName", projectName );

    return true;
}

void GeneralPage::openWorkspaceDirectory()
{
    QString workspace = QFileDialog::getExistingDirectory( this,
                                                           "Choose a workspace directory",
                                                           QDir::homePath() );
    if ( workspace.isEmpty() ) return;
    ui.lineEditWorkspace->setText( workspace );
}

void GeneralPage::updateProjectLocation()
{
    QString workspacePath = ui.lineEditWorkspace->text();
    if ( workspacePath.isEmpty() )
    {
        ui.lineEditProjectLocation->setText( tr( "Missing workspace location" ) );
        ui.lineEditProjectLocation->setPalette( pInvalid );
    }
    else
    {
        QString pName = ui.lineEditName->text().replace( ' ', '_' );
        QDir workspaceDir( workspacePath );
        QDir projectDir( QString( "%1/%2" ).arg( workspacePath, pName ) );

        ui.lineEditProjectLocation->setText( projectDir.absolutePath() );

        if ( workspaceDir.isRelative() )
        {
            ui.lineEditProjectLocation->setPalette( pInvalid );
            ui.lineEditProjectLocation->setText( tr( "Invalid workspace location" ) );
            return;
        }

        if ( !workspaceDir.exists() )
            ui.lineEditProjectLocation->setPalette( pInvalid );
        else
        {
            if ( projectDir.exists() )
                ui.lineEditProjectLocation->setPalette( pInvalid );
            else
                ui.lineEditProjectLocation->setPalette( pValid );
        }
    }
}

void GeneralPage::setWorkspaceStatus( bool valid )
{

}
