#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"

PreviewWidget::PreviewWidget( QWidget *parent ) :
    QDialog( parent ),
    m_ui( new Ui::PreviewWidget )
{
    m_ui->setupUi( this );
}

PreviewWidget::~PreviewWidget()
{
    delete m_ui;
}

void PreviewWidget::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}
