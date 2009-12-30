/*****************************************************************************
 * GraphicsMovieItem.cpp: Represent a movie graphically in the timeline
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include <QPainter>
#include <QLinearGradient>
#include <QDebug>
#include <QTime>
#include "GraphicsMovieItem.h"
#include "TracksView.h"
#include "Timeline.h"

GraphicsMovieItem::GraphicsMovieItem( Clip* clip ) : m_clip( clip ), m_width( 0 ), m_height( 0 )
{
    setFlags( QGraphicsItem::ItemIsSelectable );

    QTime length = QTime().addMSecs( clip->getParent()->getLengthMS() );
    QString tooltip( tr( "<p style='white-space:pre'><b>Name:</b> %1"
                     "<br><b>Length:</b> %2" )
                     .arg( clip->getParent()->getFileName() )
                     .arg( length.toString("hh:mm:ss.zzz") ) );
    setToolTip( tooltip );
    setAcceptHoverEvents( true );

    // Adjust the width
    setWidth( clip->getLength() );
    // Automatically adjust future changes
    connect( clip, SIGNAL( lengthUpdated() ), this, SLOT( adjustLength() ) );
}

GraphicsMovieItem::~GraphicsMovieItem()
{
}

QRectF GraphicsMovieItem::boundingRect() const
{
    return QRectF( 0, 0, m_width, m_height );
}

void GraphicsMovieItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    painter->save();
    paintRect( painter, option );
    painter->restore();

    painter->save();
    paintTitle( painter, option );
    painter->restore();

    /* buggy code
    painter->save();
    paintAudioSpectrum( painter );
    painter->restore();
    */
}

void GraphicsMovieItem::setWidth( int width )
{
    prepareGeometryChange();
    m_width = width;
}

void GraphicsMovieItem::setHeight( int height )
{
    prepareGeometryChange();
    m_height = height;
}

void GraphicsMovieItem::adjustLength()
{
    //FIXME implement clip expanding.
    Q_ASSERT_X( m_clip->getLength() <= m_width, "adjustLength", "Clip expanding not supported!" );
    prepareGeometryChange();
    setWidth( m_clip->getLength() );
}

void GraphicsMovieItem::paintAudioSpectrum( QPainter* painter )
{
    qreal x1, x2, y1, y2;
    QRectF tmp = boundingRect();
    tmp.getCoords( &x1, &y1, &x2, &y2 );

    painter->setPen( Qt::black );
    painter->setBrush( Qt::black );

    QLineF line;

    for (int i = 0; i < m_clip->getParent()->getAudioFrameList().size(); i++)
    {
        //qDebug() << "Frame: " << i << "/" << m_media->getAudioFrameList().size();
        for (unsigned int u = 0; u < m_clip->getParent()->getAudioNbSample(); u += 400)
        {


            int value = m_clip->getParent()->getAudioFrameList()[i][u];
            value /= 30;
            if( value > 48 ) value = 48;
            if( value < 0 ) value = 0;

            //qDebug() << "frame nb:" << i << " buff value: " << value;

            static QPointF point = QPointF( x1, ( y2 - y1 ) / 2 );

            qreal y = ( ( y2 - y1 ) / 2 ) - value / 2;
            x1 += 2;
            QPointF point2( x1, y );

            line.setPoints( point, point2);
            //line.setLine( x1 , y, x1 + 2, y);


            painter->drawLine(line);
            //painter->drawRect(tmp);

            point.setX( point2.x() );
            point.setY( point2.y() );
        }
    }
}

Clip* GraphicsMovieItem::clip() const
{
    return m_clip;
}

void GraphicsMovieItem::paintRect( QPainter* painter, const QStyleOptionGraphicsItem* option )
{
    QRectF drawRect;
    bool drawRound;

    // Disable the matrix transformations
    painter->setWorldMatrixEnabled( false );

    painter->setRenderHint( QPainter::Antialiasing );

    // Get the transformations required to map the text on the viewport
    QTransform viewPortTransform = Timeline::getInstance()->tracksView()->viewportTransform();

    // Determine if a drawing optimization can be used
    if ( option->exposedRect.left() > ROUNDED_RECT_RADIUS &&
         option->exposedRect.right() < boundingRect().right() - ROUNDED_RECT_RADIUS )
    {
        // Optimized: paint only the exposed (horizontal) area
        drawRect = QRectF( option->exposedRect.left(),
                           boundingRect().top(),
                           option->exposedRect.right(),
                           boundingRect().bottom() );
        drawRound = false;
    }
    else
    {
        // Unoptimized: the item must be fully repaint
        drawRect = boundingRect();
        drawRound = true;
    }

    // Do the transformation
    QRectF mapped = deviceTransform( viewPortTransform ).mapRect( drawRect );

    QLinearGradient gradient( mapped.topLeft(), mapped.bottomLeft() );
    gradient.setColorAt( 0, QColor::fromRgb( 78, 78, 78 ) );
    gradient.setColorAt( 0.4, QColor::fromRgb( 72, 72, 72 ) );
    gradient.setColorAt( 0.4, QColor::fromRgb( 50, 50, 50 ) );
    gradient.setColorAt( 1, QColor::fromRgb( 45, 45, 45 ) );

    painter->setPen( Qt::NoPen );
    painter->setBrush( QBrush( gradient ) );

    if ( drawRound )
        painter->drawRoundedRect( mapped, ROUNDED_RECT_RADIUS, ROUNDED_RECT_RADIUS );
    else
        painter->drawRect( mapped );

    if ( isSelected() )
    {
        setZValue( Z_SELECTED );
        painter->setPen( Qt::yellow );
        painter->setBrush( Qt::NoBrush );
        mapped.adjust( 0, 0, 0, -1 );
        if ( drawRound )
            painter->drawRoundedRect( mapped, ROUNDED_RECT_RADIUS, ROUNDED_RECT_RADIUS );
        else
            painter->drawRect( mapped );
    }
    else
        setZValue( Z_NOT_SELECTED );
}

void GraphicsMovieItem::paintTitle( QPainter* painter, const QStyleOptionGraphicsItem* option )
{
    Q_UNUSED( option );

    // Disable the matrix transformations
    painter->setWorldMatrixEnabled( false );

    // Setup the font
    QFont f = painter->font();
    f.setPointSize( 8 );
    painter->setFont( f );

    // Initiate the font metrics calculation
    QFontMetrics fm( painter->font() );
    QString text = m_clip->getParent()->getFileName();

    // Get the transformations required to map the text on the viewport
    QTransform viewPortTransform = Timeline::getInstance()->tracksView()->viewportTransform();
    // Do the transformation
    QRectF mapped = deviceTransform( viewPortTransform ).mapRect( boundingRect() );
    // Create an inner rect
    mapped.adjust( 2, 2, -2, -2 );

    painter->setPen( Qt::white );
    painter->drawText( mapped, Qt::AlignVCenter, fm.elidedText( text, Qt::ElideRight, mapped.width() ) );
}

void GraphicsMovieItem::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
    TracksView* tv = Timeline::getInstance()->tracksView();
    if ( tv )
    {
        switch ( tv->tool() )
        {
            case TOOL_DEFAULT:
            setCursor( Qt::OpenHandCursor );
            break;

            case TOOL_CUT:
            setCursor( QCursor( QPixmap( ":/images/editcut" ) ) );
            break;
        }
    }

    AbstractGraphicsMediaItem::hoverEnterEvent( event );
}

void GraphicsMovieItem::hoverLeaveEvent( QGraphicsSceneHoverEvent* event )
{
    AbstractGraphicsMediaItem::hoverLeaveEvent( event );
}

void GraphicsMovieItem::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    TracksView* tv = Timeline::getInstance()->tracksView();
    if ( tv->tool() == TOOL_DEFAULT )
        setCursor( Qt::ClosedHandCursor );
    else if ( tv->tool() == TOOL_CUT )
        emit split( this, qRound64( event->pos().x() ) );
}

void GraphicsMovieItem::mouseReleaseEvent( QGraphicsSceneMouseEvent*  event )
{
    Q_UNUSED( event );
    TracksView* tv = Timeline::getInstance()->tracksView();
    if ( tv->tool() == TOOL_DEFAULT )
        setCursor( Qt::OpenHandCursor );
}
