#include "MediaLibraryWidget.h"

MediaLibraryWidget::MediaLibraryWidget( QWidget* parent ) : QWidget( parent )
{
    m_nav = new StackViewController( this );
    MediaListViewController* list = new MediaListViewController( m_nav );

    m_nav->pushViewController( list );
}

MediaLibraryWidget::~MediaLibraryWidget()
{

}

const ViewController*       MediaLibraryWidget::getCurrentViewController()
{
    return m_nav->getCurrentViewController();
}
