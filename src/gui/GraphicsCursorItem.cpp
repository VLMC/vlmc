#include "GraphicsCursorItem.h"

GraphicsCursorItem::GraphicsCursorItem( int height, const QPen& pen )
        : m_height( height ), m_pen( pen )
{
    setFlags( QGraphicsItem::ItemIgnoresTransformations );
    setZValue( 100 );
    setCursor( QCursor( Qt::SizeHorCursor ) );

    m_boundingRect = QRectF( -2, 0, 3, m_height );
}

QRectF GraphicsCursorItem::boundingRect() const
{
    return m_boundingRect;
}

void GraphicsCursorItem::paint( QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* )
{
    painter->setPen( m_pen );
    painter->drawLine( 0, 0, 0, m_height );
}
