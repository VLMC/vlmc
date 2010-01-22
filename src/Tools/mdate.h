/*****************************************************************************
 * mdate.h: high resolution time management functions
 * Extracted from vlc mtime.c
 *****************************************************************************
 * Copyright (C) 1998-2007 the VideoLAN team
 * Copyright © 2006-2007 Rémi Denis-Courmont
 * $Id: 454e99fb27a5ac3b415d92ab98d2f39245dbea2b $
 *
 * Authors: Vincent Seguin <seguin@via.ecp.fr>
 *          Rémi Denis-Courmont <rem$videolan,org>
 *          Gisle Vanem
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef MDATE_H
#define MDATE_H

#include <QtGlobal>
typedef qint64 mtime_t;

extern "C" mtime_t mdate( void );

#endif // MDATE_H
