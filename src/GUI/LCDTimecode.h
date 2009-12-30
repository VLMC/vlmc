/*****************************************************************************
 * LCDTimecode.h: Widget that displays an LCD-like timecode
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

#ifndef LCDTIMECODE_H
#define LCDTIMECODE_H

#include <QLCDNumber>

class LCDTimecode : public QLCDNumber
{
    Q_OBJECT
public:
    LCDTimecode( QWidget* parent = 0 );

public slots:
    void setTime( int hours, int minutes = 0, int seconds = 0, int frames = 0 );
};

#endif // LCDTIMECODE_H
