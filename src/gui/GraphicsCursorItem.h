#ifndef GRAPHICSCURSORITEM_H
#define GRAPHICSCURSORITEM_H

#include <QGraphicsItem>
#include <QPen>
#include <QRectF>
#include <QPainter>
#include <QCursor>

class GraphicsCursorItem : public QGraphicsItem
{
public:
    GraphicsCursorItem( int height, const QPen& pen );

protected:
    virtual QRectF boundingRect() const;
    virtual void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0 );

private:
    int m_height;
    QPen m_pen;
    QRectF m_boundingRect;
};

#endif // GRAPHICSCURSORITEM_H
