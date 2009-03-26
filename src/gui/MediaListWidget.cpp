

#include "MediaListWidget.h"

MediaListWidget::MediaListWidget( QWidget* parent ) : QListWidget( parent )
{
}

void    MediaListWidget::setType( QString fileType )
{
    this->m_Type = fileType;
}

void    MediaListWidget::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton)
        this->m_dragStartPos = event->pos();
    QListWidget::mousePressEvent( event );
}

void    MediaListWidget::mouseMoveEvent( QMouseEvent* event )
{
    if ( this->currentItem() == NULL )
        return;
    if ( !( event->buttons() & Qt::LeftButton ) )
         return;
    if ( ( event->pos() - m_dragStartPos ).manhattanLength()
          < QApplication::startDragDistance() )
        return;

    QMimeData *mimeData = new QMimeData;
    mimeData->setText( ( ( ListViewMediaItem* )( currentItem() ))->fileInfo->absoluteFilePath() );
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
}
