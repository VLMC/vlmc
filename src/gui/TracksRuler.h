/*****************************************************************************
 * TracksRuler.h: Timeline ruler
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

#ifndef TRACKSRULER_H
#define TRACKSRULER_H

#include <QWidget>
#include <QPaintEvent>
#include <QString>

const int FRAME_SIZE = 90;
static const int FIX_WIDTH = 24; /* widget width in pixel */
static const int LINE_END = (FIX_WIDTH - 3);
static const int END_MARK_LENGTH = (FIX_WIDTH - 8);
static const int BIG_MARK_LENGTH = (END_MARK_LENGTH * 3 / 4);
static const int BIG_MARK_X2 = LINE_END;
static const int BIG_MARK_X1 = (BIG_MARK_X2 - BIG_MARK_LENGTH);
static const int MIDDLE_MARK_LENGTH = (END_MARK_LENGTH / 2);
static const int MIDDLE_MARK_X2 = LINE_END;
static const int MIDDLE_MARK_X1 = (MIDDLE_MARK_X2 - MIDDLE_MARK_LENGTH);
static const int LITTLE_MARK_LENGTH = (MIDDLE_MARK_LENGTH / 2);
static const int LITTLE_MARK_X2 = LINE_END;
static const int LITTLE_MARK_X1 = (LITTLE_MARK_X2 - LITTLE_MARK_LENGTH);

static const int LABEL_SIZE = 9;

class TracksRuler : public QWidget
{
    Q_OBJECT

public:
    TracksRuler( QWidget* parent = 0 );
    void setDuration( int duration );
    void setPixelPerMark( double rate );
    static const int comboScale[];

public slots:
    void moveRuler( int pos );

protected:
    virtual void paintEvent( QPaintEvent* e );

private:
    QString getTimeCode( int frames ) const;
    int m_duration;
    int m_fps;
    int m_offset;
    double m_factor;
    double m_scale;
    double m_textSpacing;
    int m_littleMarkDistance;
    int m_mediumMarkDistance;
    int m_bigMarkDistance;
};

#endif // TRACKSRULER_H
