#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "WelcomePage.h"

WelcomePage::WelcomePage( QWidget* parent )
    : QWizardPage( parent )
{
    m_ui.setupUi( this );

    setTitle( tr( "Project wizard" ) );
    setSubTitle( tr( "Open or create a project" ) );
}

int WelcomePage::nextId() const
{
    if ( m_ui.createRadioButton->isChecked() )
        return ProjectWizard::Page_General;
    else
        return ProjectWizard::Page_Open;
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
