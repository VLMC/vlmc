#include "Import.h"
#include "ui_Import.h"
#include "ClipRenderer.h"
#include "Library.h"

Import::Import(QWidget *parent) :
    QDialog( parent ), m_ui( new Ui::Import )
{
    m_ui->setupUi( this );
    m_importBrowser = new ImportBrowser( m_ui->ImportBrowserWidget );
    m_previewWidget = new PreviewWidget( new ClipRenderer, m_ui->PreviewWidget );

    m_ui->ImportBrowserWidget = m_importBrowser;
    m_ui->PreviewWidget = m_previewWidget;

    connect( m_importBrowser, SIGNAL( mediaSelected( QFileInfo ) ), this, SLOT( setUIMetaData( QFileInfo ) ) );
    connect( this, SIGNAL( mediaSelected( Clip* ) ), m_previewWidget->getGenericRenderer(), SLOT( setClip( Clip* ) ) );
}

Import::~Import()
{
    delete m_ui;
    delete m_currentMedia;
    delete m_currentClip;
    delete m_metaDataWorker;
    delete m_importBrowser;
    delete m_previewWidget;
}

void    Import::setUIMetaData( QFileInfo fileInfos )
{
    m_ui->nameValueLabel->setText( fileInfos.fileName() );
    m_currentMedia = new Media( fileInfos.filePath(), fileInfos.fileName() );

    m_metaDataWorker = new MetaDataWorker( m_currentMedia );
    m_metaDataWorker->start();

    QVariant fps( m_currentMedia->getFps() );
    QVariant height( m_currentMedia->getHeight() );
    QVariant width( m_currentMedia->getWidth() );
    QVariant length( m_currentMedia->getLength() );

    m_ui->fpsValueLabel->setText( fps.toString() );
    m_ui->durationValueLabel->setText( height.toString() );
    m_ui->resolutionValueLabel->setText( length.toString() + "x" + width.toString() );

    m_currentClip = new Clip( m_currentMedia );
    emit mediaSelected( m_currentClip );
}

void    Import::accept()
{
    for(int i = 0; i < m_importBrowser->getMediaInfoList().count(); i++ )
        Library::getInstance()->newMediaLoadingAsked( m_importBrowser->getMediaInfoList()[i].filePath(), m_importBrowser->getMediaInfoList()[i].fileName() );
    done(Accepted);
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
