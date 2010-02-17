/*****************************************************************************
 * RendererSettings.cpp: Edit the output file parameters
 *****************************************************************************
 * Copyright (C) 2008-2010 VideoLAN
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#include "RendererSettings.h"
#include "SettingsManager.h"

#include "ui_RendererSettings.h"

#include <QFileDialog>
#include <QMessageBox>

RendererSettings::RendererSettings()
{
    SettingsManager::Type xml = SettingsManager::XML;
    m_ui.setupUi( this );
    connect( m_ui.outputFileNameButton, SIGNAL(clicked() ),
             this, SLOT(selectOutputFileName() ) );
    QVariant  width = SettingsManager::getInstance()->value( "project/VideoProjectWidth",  480 ,xml );
    QVariant  height = SettingsManager::getInstance()->value( "project/VideoProjectHeight", 300, xml );
    QVariant  outputFps = SettingsManager::getInstance()->value( "global/VLMCOutPutFPS", 30, SettingsManager::QSett );
    m_ui.width->setValue( width.toInt() );
    m_ui.height->setValue( height.toInt() );
    m_ui.fps->setValue( outputFps.toDouble() );
}

void
RendererSettings::selectOutputFileName()
{
    QString outputFileName =
            QFileDialog::getSaveFileName( NULL, tr ( "Enter the output file name" ),
                                          QDir::currentPath(), "Videos(*.avi *.mpg)" );
    m_ui.outputFileName->setText( outputFileName );
}

void
RendererSettings::accept()
{
    if ( width() <= 0 || height() <= 0 || outputFileName().isEmpty() == true ||
         fps() <= .0f )
    {
        QMessageBox::warning( this, "Invalid parameters",
                              "Please enter valid rendering parameters" );
    }
    else
        QDialog::accept();
}

quint32
RendererSettings::width() const
{
    return m_ui.width->value();
}

quint32
RendererSettings::height() const
{
    return m_ui.height->value();
}

double
RendererSettings::fps() const
{
    return m_ui.fps->value();
}

quint32
RendererSettings::videoBitrate() const
{
    return m_ui.videoQuality->value();
}

quint32
RendererSettings::audioBitrate() const
{
    return m_ui.audioQuality->value();
}

QString
RendererSettings::outputFileName() const
{
    return m_ui.outputFileName->text();
}
