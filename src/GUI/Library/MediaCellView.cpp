#include "MediaCellView.h"
#include "ui_MediaCellView.h"
#include "Library.h"
#include "ClipProperty.h"

MediaCellView::MediaCellView( const QUuid& uuid, QWidget *parent ) :
        QWidget( parent ), m_ui( new Ui::MediaCellView ), m_uuid( uuid )
{
    m_ui->setupUi( this );
    setFocusPolicy( Qt::ClickFocus );
}

MediaCellView::~MediaCellView()
{
    delete m_ui;
}

void MediaCellView::changeEvent( QEvent *e )
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

void            MediaCellView::setTitle( const QString& title )
{
    m_ui->title->setText( title );
}

void            MediaCellView::setThumbnail( const QPixmap& pixmap )
{
    m_ui->thumbnail->setScaledContents( true );
    m_ui->thumbnail->setPixmap( pixmap );
}

QString  MediaCellView::title() const
{
    return m_ui->title->text();
}

const QUuid&    MediaCellView::uuid() const
{
    return m_uuid;
}

void            MediaCellView::mouseDoubleClickEvent( QMouseEvent* event )
{
    if ( ( event->buttons() | Qt::LeftButton ) == Qt::LeftButton )
    {
        ClipProperty* mp = new ClipProperty( Library::getInstance()->getClip( m_uuid ), this );
        mp->setModal( true );
        mp->show();
    }
}

void            MediaCellView::mousePressEvent( QMouseEvent* event )
{
    if ( ( event->buttons() | Qt::LeftButton ) == Qt::LeftButton )
    {
        emit cellSelected( m_uuid );
    }
}
