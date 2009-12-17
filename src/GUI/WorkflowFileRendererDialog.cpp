/*****************************************************************************
 * WorkflowFileRendererDialog.cpp: Display a render feedback.
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *          Christophe Courtaut <christophe.courtaut@gmail.com>
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

#include "WorkflowFileRendererDialog.h"

#define VIDEOWIDTH 640
#define VIDEOHEIGHT 480

#include <QDebug>

WorkflowFileRendererDialog::WorkflowFileRendererDialog()
{
    m_ui.setupUi( this );
}

void    WorkflowFileRendererDialog::setOutputFileName( const QString& outputFileName )
{
    m_ui.nameLabel->setText( outputFileName );
    setWindowTitle( "Rendering to " + outputFileName );
}

void    WorkflowFileRendererDialog::setProgressBarValue( int val )
{
    m_ui.progressBar->setValue( val );
}

void    WorkflowFileRendererDialog::updatePreview( const uchar* buff )
{
    m_ui.previewLabel->setPixmap( QPixmap::fromImage( QImage( buff, VIDEOWIDTH, VIDEOHEIGHT, QImage::Format_RGB888 ).rgbSwapped() ) );
}
