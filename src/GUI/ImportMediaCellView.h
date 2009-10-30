#ifndef IMPORTMEDIACELLVIEW_H
#define IMPORTMEDIACELLVIEW_H

#include "MediaCellView.h"

class ImportMediaCellView : public MediaCellView
{
    Q_OBJECT

    public:
        ImportMediaCellView( const QUuid& uuid, QWidget *parent = 0 );

    protected:
        void    mouseDoubleClickEvent( QMouseEvent* );
        void    mousePressEvent( QMouseEvent* );
        void    mouseMoveEvent( QMouseEvent* );
};

#endif // IMPORTMEDIACELLVIEW_H
