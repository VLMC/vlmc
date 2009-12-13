#include "MediaListViewController.h"

#include <QDebug>

MediaListViewController::MediaListViewController( StackViewController* nav ) :
        ListViewController( nav ), m_nav( nav )
{
    connect(Library::getInstance(), SIGNAL( newMediaLoaded( Media* ) ), this, SLOT( newMediaLoaded( Media* ) ) );
    m_cells = new QHash<QUuid, QWidget*>();
}

MediaListViewController::~MediaListViewController()
{
    delete m_cells;
}

void        MediaListViewController::newMediaLoaded( Media* media )
{
    MediaCellView* cell = new MediaCellView( media->getUuid() );

    connect( cell, SIGNAL ( cellSelected( QUuid ) ), this, SLOT ( cellSelection( QUuid ) ) );
    connect( cell, SIGNAL ( cellDeleted( QUuid ) ), this, SLOT( mediaDeletion( QUuid ) ) );

    if ( media->getMetadata() != Media::ParsedWithSnapshot )
        connect( media, SIGNAL( snapshotComputed( Media* ) ), this, SLOT( updateCell( Media* ) ) );
    cell->setThumbnail( media->getSnapshot() );
    cell->setTitle( media->getFileName() );
    cell->setLength( media->getLengthMS() );
    addCell(cell);
    m_cells->insert( media->getUuid(), cell );
}

void    MediaListViewController::cellSelection( const QUuid& uuid )
{
    if ( m_currentUuid == uuid )
        return;

    if ( m_cells->contains( uuid ) )
    {
        if ( !m_currentUuid.isNull() && m_cells->contains( m_currentUuid ) )
            m_cells->value( m_currentUuid )->setPalette( m_cells->value( uuid )->palette() );
        QPalette p = m_cells->value( uuid )->palette();
        p.setColor( QPalette::Window, QColor( Qt::darkBlue ) );
        m_cells->value( uuid )->setPalette( p );
        m_currentUuid = uuid;
        emit mediaSelected( Library::getInstance()->getMedia( uuid ) );
    }
}

void    MediaListViewController::mediaDeletion( const QUuid& uuid )
{
    emit mediaDeleted( uuid );
}

void    MediaListViewController::mediaRemoved( const QUuid& uuid )
{
    removeCell( m_cells->value( uuid ) );
    m_cells->remove( uuid );
    m_currentUuid = QUuid();
}

void    MediaListViewController::updateCell( Media* media )
{
    MediaCellView* cell = dynamic_cast<MediaCellView*>( m_cells->value( media->getUuid(), NULL ) );
    if ( cell != NULL )
        cell->setThumbnail( media->getSnapshot() );
}
