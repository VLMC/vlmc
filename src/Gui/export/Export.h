/*****************************************************************************
 * Export.h: Export menu
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Geoffroy Lacarriere <geoffroylaca@gmail.com>
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

#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>
#include <QListWidgetItem>

class WorkflowFileRenderer;

namespace Ui
{
    class Export;
}

enum Profile
{
    Video_H264_AAC,
    Video_Dirac_AAC,
    Video_Theora_Vorbis,
    Video_Theora_Flac,
    Video_MPEG4_AAC,
    Video_MPEG2_MPGA,
    Video_WMV_WMA,
    Audio_Vorbis,
    Audio_MP3,
    Audio_AAC,
    Audio_FLAC
};

class Export : public QDialog
{
    Q_OBJECT
    public:
        Export( QWidget *parent = 0 );
        ~Export();
        const QString&  exportParam() const { return m_exportParam; }

    protected:
        void changeEvent( QEvent *e );
        void accept();
        void reject();

    private:
        Ui::Export              *m_ui;
        QString                 m_exportParam;
        WorkflowFileRenderer    *m_renderer;

    public slots:
        void onIndexChanged( int index );
};

#endif // EXPORT_H
