#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "LoadPage.h"

LoadPage::LoadPage( QWidget* parent )
    : QWizardPage( parent )
{
    build();
    if ( parent != 0 )
    {
        QObject::connect( this,
                          SIGNAL( projectButtonClicked() ),
                          parent,
                          SLOT( loadProject() ) );
    }
}

LoadPage::~LoadPage() {}

void    LoadPage::build()
{
    QLabel*         label = new QLabel( this );
    QPushButton*    butt = new QPushButton( this );
    QVBoxLayout*    lay = new QVBoxLayout( this );

    QObject::connect( butt,
                      SIGNAL( clicked() ),
                      this,
                      SLOT( loadProjectClicked() ) );

    label->setText( tr( "Welcome to the project Wizard.\n"
                    "This wizard will allow you to create"
                    "a new project, or to open an existing project.\n"
                    "To open an existing project, click on the button below,"
                    "or click next to create a new project" ) );
    butt->setText( tr( "Open Existing Project" ) );

    lay->addWidget( label );
    lay->addWidget( butt );
    setLayout( lay );

    return ;
}

void    LoadPage::loadProjectClicked()
{
    emit projectButtonClicked();
    return ;
}
