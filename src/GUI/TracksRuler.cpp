/*****************************************************************************
 * TracksRuler.cpp: Timeline ruler
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

#include <QStylePainter>
#include <QBrush>
#include <QColor>
#include <QPalette>
#include <QPolygon>
#include "TracksRuler.h"
#include "TracksView.h"
#include "SettingsManager.h"

const int TracksRuler::comboScale[] = { 1, 2, 5, 10, 25, 50, 125, 250, 500, 725, 1500, 3000, 6000, 12000};

TracksRuler::TracksRuler( TracksView* tracksView, QWidget* parent )
    : QWidget( parent ), m_tracksView( tracksView ), m_duration ( 0 ), m_offset( 0 )
{

    //TODO We should really get that from the
    // workflow and not directly from the settings.
    SettingsManager* settings = SettingsManager::getInstance();
    m_fps = qRound( settings->getValue( "VLMC", "VLMCOutPutFPS" )->get().toDouble() );

    m_factor = 1;
    m_scale = 3;
    m_littleMarkDistance = FRAME_SIZE;
    m_mediumMarkDistance = FRAME_SIZE * m_fps;
    m_bigMarkDistance = FRAME_SIZE * m_fps * 60;
    setMinimumHeight( 30 );
    setPixelPerMark( 5 );

    // Redraw the ruler when the cursor position change
    connect( tracksView->tracksCursor(), SIGNAL( cursorPositionChanged(qint64) ),
             this, SLOT( update() ) );
}

void TracksRuler::setPixelPerMark( double rate )
{
    int scale = comboScale[ ( int )rate ];
    m_factor = 1.0 / ( double ) scale * FRAME_SIZE;
    m_scale = 1.0 / ( double ) scale;
    double fend = m_scale * m_littleMarkDistance;

    switch ( ( int )rate )
    {
    case 0:
        m_textSpacing = fend;
        break;
    case 1:
        m_textSpacing = fend * m_fps;
        break;
    case 2:
    case 3:
    case 4:
        m_textSpacing = fend * m_fps * 5;
        break;
    case 5:
    case 6:
        m_textSpacing = fend * m_fps * 10;
        break;
    case 7:
        m_textSpacing = fend * m_fps * 30;
        break;
    case 8:
    case 9:
    case 10:
        m_textSpacing = fend * m_fps * 60;
        break;
    case 11:
    case 12:
        m_textSpacing = fend * m_fps * 300;
        break;
    case 13:
        m_textSpacing = fend * m_fps * 600;
        break;
    }
    update();
}

void TracksRuler::setDuration( int duration )
{
    int oldDuration = m_duration;
    m_duration = duration;

    Q_UNUSED( oldDuration );
    //FIXME The optimized update() version cause wrong values to be shown in
    //the ruler. I don't understand what's happening here.

    /*update( ( int ) ( qMin( oldDuration, m_duration ) * m_factor - 1 - m_offset ), 0,
            ( int ) ( qAbs( oldDuration - m_duration ) * m_factor + 2 ), height() );*/
    update();
}

void TracksRuler::paintEvent( QPaintEvent* e )
{
    QStylePainter painter( this );
    painter.setClipRect( e->rect() );

    // Draw the background
    const int projectEnd = ( int )( m_duration * m_factor );
    if ( projectEnd - m_offset > 1 )
        painter.fillRect( 0, 0, projectEnd - m_offset, height(), QBrush( QColor( 245, 245, 245 ) ) );


    double f, step;
    int offsetMax;
    int offsetMin;
    const int maxVal = ( e->rect().right() + m_offset ) / FRAME_SIZE + 1;
    offsetMax = maxVal * FRAME_SIZE;

    QPalette palette;
    painter.setPen( palette.dark().color() );
    offsetMin = ( int ) ( ( e->rect().left() + m_offset ) / m_textSpacing );
    offsetMin = ( int ) ( offsetMin * m_textSpacing );
    for ( f = offsetMin; f < offsetMax; f += m_textSpacing )
    {
        QString time = getTimeCode( (int)( f / m_factor + 0.5 ) );
        painter.drawText( ( int )f - m_offset + 2, LABEL_SIZE + 1, time );
    }


    // Draw the marks
    offsetMin = ( e->rect().left() + m_offset ) / m_littleMarkDistance;
    offsetMin = offsetMin * m_littleMarkDistance;
    step = m_scale * m_littleMarkDistance;
    if ( step > 5 )
        for ( f = offsetMin - m_offset; f < offsetMax - m_offset; f += step )
            painter.drawLine( ( int )f, LITTLE_MARK_X1, ( int )f, LITTLE_MARK_X2 );

    offsetMin = ( e->rect().left() + m_offset ) / m_mediumMarkDistance;
    offsetMin = offsetMin * m_mediumMarkDistance;
    step = m_scale * m_mediumMarkDistance;
    if ( step > 5 )
        for ( f = offsetMin - m_offset; f < offsetMax - m_offset; f += step )
            painter.drawLine( ( int )f, MIDDLE_MARK_X1, ( int )f, MIDDLE_MARK_X2 );

    offsetMin = ( e->rect().left() + m_offset ) / m_bigMarkDistance;
    offsetMin = offsetMin * m_bigMarkDistance;
    step = m_scale * m_bigMarkDistance;
    if ( step > 5 )
        for ( f = offsetMin - m_offset; f < offsetMax - m_offset; f += step )
            painter.drawLine( ( int )f, BIG_MARK_X1, ( int )f, BIG_MARK_X2 );

    // Draw the pointer
    int cursorPos = qRound( m_tracksView->cursorPos() * m_factor - offset() );
    QPolygon cursor( 3 );
    cursor.setPoints( 3, cursorPos - 9, 11, cursorPos + 9, 11, cursorPos, 30 );
    painter.setBrush( QBrush( QColor( 82, 97, 122, 150 ) ) );
    painter.drawPolygon( cursor );
}

void TracksRuler::mousePressEvent( QMouseEvent* event )
{
    if ( event->buttons() == Qt::LeftButton &&
         event->modifiers() == Qt::NoModifier )
    {
        emit frameChanged( qMax( (qreal)0, (qreal)( event->x() + offset() ) / m_factor ), MainWorkflow::RulerCursor );
    }
}

void TracksRuler::mouseMoveEvent( QMouseEvent* event )
{
    if ( event->buttons() == Qt::LeftButton &&
         event->modifiers() == Qt::NoModifier )
    {
        emit frameChanged( qMax( (qreal)0, (qreal)( event->x() + offset() ) / m_factor ), MainWorkflow::RulerCursor );
    }
}

void        TracksRuler::moveRuler( int pos )
{
    m_offset = pos;
    update();
}

QString     TracksRuler::getTimeCode( int frames ) const
{
    int seconds = frames / m_fps;
    frames = frames % m_fps;

    int minutes = seconds / 60;
    seconds = seconds % 60;
    int hours = minutes / 60;
    minutes = minutes % 60;

    QString text;

    text.append( QString::number( hours ).rightJustified( 2, '0', false ) );
    text.append( ':' );
    text.append( QString::number( minutes ).rightJustified( 2, '0', false ) );
    text.append( ':' );
    text.append( QString::number( seconds ).rightJustified( 2, '0', false ) );
    text.append( ':' );
    text.append( QString::number( frames ).rightJustified( 2, '0', false ) );
    return text;
}
