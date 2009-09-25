#include "MediaLibraryWidget.h"

MediaLibraryWidget::MediaLibraryWidget( QWidget* parent ) : QWidget( parent )
{
    StackViewController* nav = new StackViewController( this );
    MediaListViewController* list = new MediaListViewController( nav );

    nav->pushViewController( list );
}

MediaLibraryWidget::~MediaLibraryWidget()
{

}
