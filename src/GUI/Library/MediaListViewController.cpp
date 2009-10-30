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

    cell->setThumbnail( media->getSnapshot() );
    cell->setTitle( media->getFileName() );
    addCell(cell);
    m_cells->insert( media->getUuid(), cell );
}

void    MediaListViewController::cellSelection( const QUuid& uuid )
{
    if ( m_currentUuid == uuid )
        return;

    if ( !m_currentUuid.isNull() )
        m_cells->value( m_currentUuid )->setPalette( m_cells->value( uuid )->palette() );
    QPalette p = m_cells->value( uuid )->palette();
    p.setColor( QPalette::Window, QColor( Qt::darkBlue ) );
    m_cells->value( uuid )->setPalette( p );
    m_currentUuid = uuid;
    emit mediaSelected( Library::getInstance()->getMedia( uuid) );
}
