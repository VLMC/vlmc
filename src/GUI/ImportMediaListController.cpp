#include "ImportMediaListController.h"

#include <QDebug>

ImportMediaListController::ImportMediaListController( StackViewController* nav ) : ListViewController( nav ), m_nav( nav )
{
    m_mediaCellList = new QHash<QUuid, ImportMediaCellView*>();
}

ImportMediaListController::~ImportMediaListController()
{
    delete m_mediaCellList;
}

void    ImportMediaListController::addMedia( Media* media )
{
    ImportMediaCellView* cell = new ImportMediaCellView( media->getUuid() );

    cell->setTitle( media->getFileName() );
    cell->setThumbnail( media->getSnapshot() );
    addCell( cell );

    m_mediaCellList->insert( media->getUuid(), cell );
}

void    ImportMediaListController::metaDataComputed( Media* media )
{
    m_mediaCellList->value( media->getUuid() )->setThumbnail( media->getSnapshot() );
}

ImportMediaCellView* ImportMediaListController::getCell( QUuid uuid ) const
{
    return m_mediaCellList->value( uuid );
}
