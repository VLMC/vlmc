#include "StackViewNavController.h"
#include "ui_StackViewNavController.h"

StackViewNavController::StackViewNavController( QWidget *parent ) :
        QWidget( parent ),
    m_ui(new Ui::StackViewNavController)
{
    m_ui->setupUi(this);
    m_ui->previousButton->setHidden(true);
}

StackViewNavController::~StackViewNavController()
{
    delete m_ui;
}

void StackViewNavController::changeEvent( QEvent *e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void    StackViewNavController::setTitle( const QString& title )
{
    m_ui->title->setText( title );
}

QPushButton*    StackViewNavController::previousButton() const
{
    return m_ui->previousButton;
}
