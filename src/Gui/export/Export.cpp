/*****************************************************************************
 * Export.cpp: Export menu
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

#include "ui_Export.h"
#include "Export.h"
#include "WorkflowFileRenderer.h"

#include <QDialogButtonBox>
#include <QDebug>
#include <QFileDialog>

Export::Export( QWidget *parent ) :
    QDialog( parent ), m_ui( new Ui::Export ), m_renderer( NULL )
{
    m_ui->setupUi( this );
    m_ui->buttonBox->addButton( tr( "Export" ), QDialogButtonBox::AcceptRole );
    connect( m_ui->listWidget,
             SIGNAL( currentRowChanged( int ) ),
             this,
             SLOT( onIndexChanged( int ) ) );
}

Export::~Export()
{
    delete m_ui;
    delete m_renderer;
}

void
Export::accept()
{
    QString outputFileName =
            QFileDialog::getSaveFileName( NULL, "Enter the output file name",
                                          QDir::currentPath(), "Videos(*.avi *.mpg)" );
    if ( outputFileName.length() == 0 || m_exportParam.isEmpty() )
        done( Rejected );
    else
    {
        if ( m_renderer )
            delete m_renderer;

        m_exportParam += outputFileName + "\"}";

        m_renderer = new WorkflowFileRenderer( outputFileName );
        m_renderer->setParam( m_exportParam );
        m_renderer->initializeRenderer();
        m_renderer->run();
    }
    done( Accepted );
}

void
Export::reject()
{
    done( Rejected );
}

void
Export::onIndexChanged( int index )
{
    QString vcodec, vb("800"), acodec, ab("128");

    switch( m_ui->listWidget->currentRow() )
    {
    case Video_H264_AAC:
        vcodec = "h264";
        acodec = "a52";
        break;
    case Video_Dirac_AAC:
        vcodec = "drac";
        acodec = "a52";
        break;
    case Video_Theora_Vorbis:
        vcodec = "theo";
        acodec = "vorb";
        break;
    case Video_Theora_Flac:
        vcodec = "theo";
        acodec = "flac";
        break;
    case Video_MPEG4_AAC:
        vcodec = "mp4v";
        acodec = "a52 ";
        break;
    case Video_MPEG2_MPGA:
        vcodec = "mpgv";
        acodec = "mpga";
        break;
    case Video_WMV_WMA:
        vcodec = "wmv3";
        acodec = "wma2";
        break;
    case Audio_Vorbis:
        vcodec = "";
        acodec = "vorb";
        break;
    case Audio_MP3:
        vcodec = "";
        acodec = "mp3";
        break;
    case Audio_AAC:
        vcodec = "";
        acodec = "a52";
        break;
    case Audio_FLAC:
        vcodec = "";
        acodec = "flac";
        break;
    default:
        return;
    }
    m_exportParam = ":sout=#transcode{vcodec=" + vcodec + ",vb=" + vb +
                               ",acodec=" + acodec + ",ab=" + ab + ",no-hurry-up}"
                               ":standard{access=file,mux=ps,dst=\"";
}

void
Export::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
        case QEvent::LanguageChange:
            m_ui->retranslateUi( this );
            break;
        default:
            break;
    }
}
