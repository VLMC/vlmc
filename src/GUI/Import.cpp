#include "Import.h"
#include "ui_Import.h"
#include "ClipRenderer.h"

Import::Import(QWidget *parent) :
    QDialog( parent ), m_ui( new Ui::Import )
{
    m_ui->setupUi( this );
    m_importBrowser = new ImportBrowser( m_ui->ImportBrowserWidget );
    m_previewWidget = new PreviewWidget( new ClipRenderer, m_ui->PreviewWidget );

    m_ui->ImportBrowserWidget = m_importBrowser;
    m_ui->PreviewWidget = m_previewWidget;

    connect( m_importBrowser, SIGNAL( mediaSelected( QFileInfo ) ), this, SLOT( setUIMetaData( QFileInfo ) ) );
}

Import::~Import()
{
    delete m_ui;
}

void    Import::setUIMetaData( QFileInfo fileInfos )
{
    m_ui->nameValueLabel->setText( fileInfos.fileName() );
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
