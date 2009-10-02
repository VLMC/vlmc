#include "MediaListViewController.h"

MediaListViewController::MediaListViewController( StackViewController* nav ) :
        ListViewController( nav ), m_nav( nav )
{
    connect(Library::getInstance(), SIGNAL( newMediaLoaded( Media* ) ), this, SLOT( newMediaLoaded( Media* ) ) );
}

MediaListViewController::~MediaListViewController()
{

}

void        MediaListViewController::newMediaLoaded( Media* media )
{
    MediaCellView* cell = new MediaCellView( media->getUuid() );

    connect( cell, SIGNAL ( cellSelected( QUuid ) ),
             this, SLOT ( cellSelected( QUuid ) ) );

    cell->setThumbnail( media->getSnapshot() );
    cell->setTitle( media->getFileName() );
    addCell(cell);
}
