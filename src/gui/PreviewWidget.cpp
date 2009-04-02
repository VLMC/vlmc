#include "PreviewWidget.h"
#include "ui_PreviewWidget.h"
#include "MediaListWidget.h"

PreviewWidget::PreviewWidget( QWidget *parent ) :
    QDialog( parent ),
    m_ui( new Ui::PreviewWidget ), m_currentMedia( NULL )
{
    m_ui->setupUi( this );

    char const *vlc_argv[] =
        {
            "-verbose", "3",
            "--no-skip-frames",
            //"--plugin-path", VLC_TREE "/modules",
            //"--ignore-config", //Don't use VLC's config files
        };
    int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );
    setAcceptDrops(true);
    m_currentInstance = new LibVLCpp::Instance( vlc_argc, vlc_argv );
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

void    PreviewWidget::dragEnterEvent( QDragEnterEvent* event )
{
    event->accept();
}

void    PreviewWidget::dropEvent( QDropEvent* event )
{
    QListWidget*    listWidget = reinterpret_cast<QListWidget*>( event->source() );
    ListViewMediaItem* item = dynamic_cast<ListViewMediaItem*>( listWidget->currentItem() );
    if ( item == NULL )
        return ;
    m_currentMedia = new InputMedia("file://" + item->fileInfo()->absoluteFilePath(), m_currentInstance );
    m_currentMedia->setupMedia();
    m_currentMedia->setDrawable( m_ui->clipRenderWidget->winId() );
    m_currentMedia->play();
}
