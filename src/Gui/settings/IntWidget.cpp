/*****************************************************************************
 * IntWidget.cpp Handle integer settings.
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@vlmc.org>
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

#include "IntWidget.h"
#include "SettingValue.h"

#include <QSpinBox>

IntWidget::IntWidget( SettingValue *s, QWidget *parent /*= NULL*/ ) :
        m_setting( s )
{
    m_spinbox = new QSpinBox( parent );
    m_spinbox->setValue( s->get().toInt() );
}

QWidget*
IntWidget::widget()
{
    return m_spinbox;
}

void
IntWidget::save()
{
    m_setting->set( m_spinbox->value() );
}
