#include "Import.h"
#include "ui_Import.h"
#include "FileBrowser.h"
#include "PreviewWidget.h"
#include "ClipRenderer.h"

Import::Import(QWidget *parent) :
    QDialog( parent ), m_ui( new Ui::Import )
{
    m_ui->setupUi( this );
    m_ui->FileBrowserWidget = new FileBrowser( m_ui->FileBrowserWidget );
    m_ui->PreviewWidget = new PreviewWidget( new ClipRenderer, m_ui->PreviewWidget );
}

Import::~Import()
{
    delete m_ui;
}

void Import::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
        case QEvent::LanguageChange:
            m_ui->retranslateUi( this );
            break;
        default:
            break;
    }
}
