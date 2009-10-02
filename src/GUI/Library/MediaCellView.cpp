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

const QPixmap*  MediaCellView::getThumbnail() const
{
    return m_ui->thumbnail->pixmap();
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
    QWidget::mousePressEvent( event );

    if ( ( event->buttons() | Qt::LeftButton ) == Qt::LeftButton )
    {
        m_dragStartPos = event->pos();
        emit cellSelected( m_uuid );
    }
}

void    MediaCellView::mouseMoveEvent( QMouseEvent* event )
{
    if ( ( event->buttons() | Qt::LeftButton ) != Qt::LeftButton )
         return;

    if ( ( event->pos() - m_dragStartPos ).manhattanLength()
          < QApplication::startDragDistance() )
        return;

    QMimeData* mimeData = new QMimeData;
    //FIXME the second argument is a media UUID instead of a Clip
    // and this is not logical... but it works.
    mimeData->setData( "vlmc/uuid", m_uuid.toString().toAscii() );
    QDrag* drag = new QDrag( this );
    drag->setMimeData( mimeData );

    drag->setPixmap( Library::getInstance()->getClip( m_uuid )->getParent()->getSnapshot().scaled( 100, 100, Qt::KeepAspectRatio ) );
    drag->exec( Qt::CopyAction | Qt::MoveAction, Qt::CopyAction );
}
