#include "Import.h"
#include "ui_Import.h"
#include "ClipRenderer.h"
#include "Library.h"
#include "QTime"

Import::Import(QWidget *parent) :
    QDialog( parent ), m_ui( new Ui::Import )
{
    m_ui->setupUi( this );
    m_importBrowser = new ImportBrowser( m_ui->ImportBrowserWidget );
    m_previewWidget = new PreviewWidget( new ClipRenderer, m_ui->PreviewWidget );

    m_ui->ImportBrowserWidget = m_importBrowser;
    m_ui->PreviewWidget = m_previewWidget;

    connect( m_importBrowser, SIGNAL( mediaSelected( QFileInfo ) ), this, SLOT( getMetaData( QFileInfo ) ) );
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

void    Import::getMetaData( QFileInfo fileInfos )
{
    m_ui->nameValueLabel->setText( fileInfos.fileName() );
    m_currentMedia = new Media( fileInfos.filePath(), fileInfos.fileName() );

    m_metaDataWorker = new MetaDataWorker( m_currentMedia );
    connect( m_metaDataWorker, SIGNAL( finished() ), this, SLOT( setUIMetaData() ) );
    m_metaDataWorker->start();
}

void    Import::setUIMetaData()
{
    m_currentClip = new Clip( m_currentMedia );

    //Duration
    QTime   duration;
    duration = duration.addSecs( m_currentClip->getLengthSecond() );
    m_ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    //Filename || title
    m_ui->nameValueLabel->setText( m_currentClip->getParent()->getFileInfo()->fileName() );
    setWindowTitle( m_currentClip->getParent()->getFileInfo()->fileName() + " " + tr( "properties" ) );
    //Resolution
    m_ui->resolutionValueLabel->setText( QString::number( m_currentClip->getParent()->getWidth() )
                                       + " x " + QString::number( m_currentClip->getParent()->getHeight() ) );
    //FPS
    m_ui->fpsValueLabel->setText( QString::number( m_currentClip->getParent()->getFps() ) );

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
