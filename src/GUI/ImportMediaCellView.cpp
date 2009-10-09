#include "ImportMediaCellView.h"

ImportMediaCellView::ImportMediaCellView( const QUuid& uuid, QWidget *parent ) : MediaCellView( uuid, parent )
{
}


void    ImportMediaCellView::mouseDoubleClickEvent( QMouseEvent* )
{

}

void    ImportMediaCellView::mousePressEvent( QMouseEvent* )
{
    emit mediaSelected( uuid() );
}
