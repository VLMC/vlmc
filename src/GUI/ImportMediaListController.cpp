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
    connect( cell, SIGNAL( cellSelected( const QUuid& ) ), this, SLOT( mediaSelection( const QUuid& ) ) );

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

void    ImportMediaListController::removeMedia( const QUuid& uuid )
{
    removeCell( m_mediaCellList->value( uuid ) );
    m_mediaCellList->remove( uuid );
}

void    ImportMediaListController::addClip( Clip* clip )
{
    ImportMediaCellView* cell = new ImportMediaCellView( clip->getUuid() );
    connect( cell, SIGNAL( cellSelected( const QUuid& ) ), this, SLOT( clipSelection( const QUuid& ) ) );

    cell->setTitle( clip->getParent()->getFileName() + " " + m_mediaCellList->size() + 1 );
    cell->setThumbnail( clip->getParent()->getSnapshot() );
    addCell( cell );

    m_mediaCellList->insert( clip->getUuid(), cell );
}

void    ImportMediaListController::removeClip( const QUuid& uuid )
{
    removeCell( m_mediaCellList->value( uuid ) );
    m_mediaCellList->remove( uuid );
}

void    ImportMediaListController::cleanAll()
{
    QUuid uuid;
    MediaCellView* cell;
    foreach( uuid, m_mediaCellList->keys() )
    {
        cell = m_mediaCellList->value( uuid );
        m_layout->removeWidget( cell );
        delete cell;
    }
    m_mediaCellList->clear();
}

void    ImportMediaListController::addClipsFromMedia( Media* media )
{
    QUuid uuid;
    foreach( uuid, media->clips()->keys() )
        addClip( media->clips()->value( uuid ) );
}

void    ImportMediaListController::mediaSelection( const QUuid& uuid )
{
    emit mediaSelected( uuid );
}

void    ImportMediaListController::clipSelection( const QUuid& uuid )
{
    emit clipSelected( uuid );
}
