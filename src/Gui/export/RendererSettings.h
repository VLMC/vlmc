/*****************************************************************************
 * RendererSettings.h: Edit the output file parameters
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

#ifndef RENDERERSETTINGS_H
#define RENDERERSETTINGS_H

#include <QDialog>
#include "ui_RendererSettings.h"

class   RendererSettings : public QDialog
{
    Q_OBJECT

    public:
        RendererSettings();
        quint32         width() const;
        quint32         height() const;
        double          fps() const;
        quint32         videoBitrate() const;
        quint32         audioBitrate() const;
        QString         outputFileName() const;

    private slots:
        void            selectOutputFileName();
        virtual void    accept();

    private:
        Ui::RendererSettings    m_ui;
};

#endif // RENDERERSETTINGS_H
