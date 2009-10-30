/*****************************************************************************
 * PreviewRuler.cpp : Slider/Ruler used into the PreviewWidget
 * with backward compatibility with QAbstractSlider.
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

#include <QDebug>
#include <QPainter>
#include <QPolygon>
#include <QBrush>
#include "PreviewRuler.h"

PreviewRuler::PreviewRuler( QWidget* parent ) : QAbstractSlider( parent ), m_renderer( NULL ), m_frame( NULL )
{
    setMouseTracking( true );
    m_isSliding = false;
    m_range = maximum() - minimum();
}

void PreviewRuler::setRenderer( GenericRenderer* renderer )
{
    if ( m_renderer )
        disconnect( m_renderer, SIGNAL( positionChanged(float) ) );
    m_renderer = renderer;

    connect( m_renderer, SIGNAL( positionChanged(float) ), this, SLOT( update() ) );
}

void PreviewRuler::sliderChange( SliderChange change )
{
    switch ( change )
    {
    case QAbstractSlider::SliderRangeChange:
        m_range = maximum() - minimum();
        break;
    case QAbstractSlider::SliderOrientationChange:
        qWarning("PreviewRuler: Slider orientation is not supported.");
        break;
    case QAbstractSlider::SliderStepsChange:
        qWarning("PreviewRuler: Slider steps are not supported.");
        break;
    case QAbstractSlider::SliderValueChange:
        m_frame = value() * m_renderer->length() / m_range;
        update();
        break;
    }
}

void PreviewRuler::paintEvent( QPaintEvent * event )
{
    Q_UNUSED( event );
    Q_ASSERT( m_renderer );

    QPainter painter( this );

    if ( m_renderer->length() > 0 )
    {
        qreal linesToDraw = 0;
        qreal spacing = 0;

        // Draw the marks

        if ( width() / 2  >= m_renderer->length() )
        {   // Every frame
            painter.setPen( QPen( Qt::cyan ) );
            linesToDraw = (qreal)m_renderer->length();
            if ( linesToDraw > 0 )
            {
                spacing = (qreal)width() / linesToDraw;
                for ( int step = 0; step < linesToDraw; ++step )
                    painter.drawLine( QLineF( step * spacing, 0, step * spacing, MARK_XSMALL ) );
            }
        }
        if ( width() / 2 >= ( m_renderer->length() / 25 ) )
        {   // Every second
            painter.setPen( QPen( Qt::green ) );
            linesToDraw = (qreal)m_renderer->length() / 25;
            if ( linesToDraw > 0 )
            {
                spacing = (qreal)width() / linesToDraw;
                for ( int step = 0; step < linesToDraw; ++step )
                    painter.drawLine( QLineF( step * spacing, 0, step * spacing, MARK_SMALL ) );
            }
        }
        else if ( width() / 2 >= ( m_renderer->length() / 25 / 12 ) )
        {   // Every 5 seconds
            painter.setPen( QPen( Qt::green ) );
            linesToDraw = (qreal)m_renderer->length() / 25 / 12;
            if ( linesToDraw > 0 )
            {
                spacing = (qreal)width() / linesToDraw;
                for ( int step = 0; step < linesToDraw; ++step )
                    painter.drawLine( QLineF( step * spacing, 0, step * spacing, MARK_SMALL) );
            }
        }
        if ( width() / 2 >= ( m_renderer->length() / 25 / 60 ) )
        {   // Every minute
            painter.setPen( QPen( Qt::yellow ) );
            linesToDraw = (qreal)m_renderer->length() / 25 / 60;
            if ( linesToDraw > 0 )
            {
                spacing = (qreal)width() / linesToDraw;
                for ( int step = 0; step < linesToDraw; ++step )
                    painter.drawLine( QLineF( step * spacing, 0, step * spacing, MARK_MEDIUM ) );

            }
        }
        else if ( width() / 2 >= ( m_renderer->length() / 25 / 60 / 12 ) )
        {   // Every 5 minutes
            painter.setPen( QPen( Qt::yellow ) );
            linesToDraw = (qreal)m_renderer->length() / 25 / 60 / 12;
            if ( linesToDraw > 0 )
            {
                spacing = (qreal)width() / linesToDraw;
                for ( int step = 0; step < linesToDraw; ++step )
                    painter.drawLine( QLineF( step * spacing, 0, step * spacing, MARK_MEDIUM ) );

            }
        }
        if ( width() / 2 >= ( m_renderer->length() / 25 / 60 / 60 ) )
        {   // Every hour
            painter.setPen( QPen( Qt::red ) );
            linesToDraw = (qreal)m_renderer->length() / 25 / 60 / 60;
            if ( linesToDraw > 0 )
            {
                spacing = (qreal)width() / linesToDraw;
                for ( int step = 0; step < linesToDraw; ++step )
                    painter.drawLine( QLineF( step * spacing, 0, step * spacing, MARK_LARGE ) );
            }
        }
    }

    // Draw the pointer
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setPen( QPen( Qt::white ) );
    QPolygon cursor( 3 );

    int cursorPos;

    if ( m_renderer->length() > 0 )
        cursorPos = m_frame * width() / m_renderer->length();
    else
        cursorPos = value() * width() / m_range;

    cursorPos = qMin( qMax( cursorPos, 0 ), width() );
    cursor.setPoints( 3, cursorPos - 5, 20, cursorPos + 5, 20, cursorPos, 9 );
    painter.setBrush( QBrush( QColor( 26, 82, 225, 255 ) ) );
    painter.drawPolygon( cursor );
}

void PreviewRuler::mousePressEvent( QMouseEvent* event )
{
    m_isSliding = true;
    if ( m_renderer->length() )
        setFrame( event->pos().x() * m_renderer->length() / width() );
    else
    {
        setValue( ( event->pos().x() * m_range / width() ) + minimum() );
        emit sliderPosChanged( ( event->pos().x() * m_range / width() ) + minimum() );
    }
}

void PreviewRuler::mouseMoveEvent( QMouseEvent* event )
{
    if ( m_isSliding )
    {
        if ( m_renderer->length() )
            setFrame( event->pos().x() * m_renderer->length() / width() );
        else
        {
            setValue( ( event->pos().x() * m_range / width() ) + minimum() );
            emit sliderPosChanged( ( event->pos().x() * m_range / width() ) + minimum() );
        }
    }
}

void PreviewRuler::mouseReleaseEvent( QMouseEvent * event )
{
    Q_UNUSED( event );
    m_isSliding = false;
}

void PreviewRuler::setFrame( qint64 frame )
{
    m_frame = frame;
    emit frameChanged( frame );
    setValue( frame * m_range / m_renderer->length() );
    if ( m_isSliding )
        emit sliderPosChanged( frame * m_range / m_renderer->length() );
    update();
}
