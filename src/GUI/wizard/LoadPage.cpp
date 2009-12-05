#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "LoadPage.h"

LoadPage::LoadPage( QWidget* parent )
    : QWizardPage( parent )
{
    m_ui.setupUi( this );

    setTitle( tr( "Project wizard" ) );
    setSubTitle( tr( "Open or create a project" ) );
}

