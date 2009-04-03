/*****************************************************************************
 * Transcode.cpp :  File Transcoder
 *****************************************************************************
 * copyright (c) 2008-2009 the vlmc team
 *
 * authors: clement chavance <chavance.c@gmail.com>
 *
 * this program is free software; you can redistribute it and/or
 * modify it under the terms of the gnu general public license
 * as published by the free software foundation; either version 2
 * of the license, or (at your option) any later version.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * gnu general public license for more details.
 *
 * you should have received a copy of the gnu general public license
 * along with this program; if not, write to the free software
 * foundation, inc., 51 franklin street, fifth floor, boston ma 02110-1301, usa.
 *****************************************************************************/

//DEBUG
#include <QtDebug>
//DEBUG
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "Transcode.h"

static bool catchVLCException(libvlc_exception_t *ex)
{
    if (libvlc_exception_raised(ex))
    {
        qDebug() << libvlc_exception_get_message(ex);
        libvlc_exception_clear(ex);
        return (true);
    }
    return (false);
}

Transcode *Transcode::m_instance = NULL;

Transcode *Transcode::instance( QWidget *parent )
{
    if (Transcode::m_instance == NULL)
        Transcode::m_instance = new Transcode( parent );
    return ( Transcode::m_instance );
}

void Transcode::changeEvent( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui.retranslateUi( this );
        break;
    default:
        QWidget::changeEvent( e );
        break;
    }
}

Transcode::Transcode( QWidget *parent )
    : QDialog( parent )
{
    m_ui.setupUi( this );
    //TODO : load every known profiles
    m_ui.profileSelector->addItem( tr( "High Quality" ), "high" );
    m_ui.profileSelector->addItem( tr( "Medium Quality" ), "medium" );
    m_ui.profileSelector->addItem( tr( "Low Quality" ), "low" );
}

void Transcode::on_browseFileButton_clicked()
{
    QString path = QFileDialog::getOpenFileName( this,
                                                 tr( "Choose File to open" ),
                                                 QDir::currentPath(),
                                                 tr( "Video files (*.avi *.mkv *.ogg)" ) );
    if ( path == "" )
        return ;
    m_origVidPath = path;
    m_ui.inputFileBox->setText( path );
}

void Transcode::m_doTranscode( const QString &outPath, const QString &transStr )
{
    char const *vlc_argv[] = 
    {
        "-verbose", "3",
    };
    int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );

    libvlc_exception_init( &m_vlcEx );
    m_libvlc = libvlc_new( vlc_argc, vlc_argv, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) )
        return ;
    m_vlcMedia = libvlc_media_new( m_libvlc, m_origVidPath.toLocal8Bit(), &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return ;
    m_vlcMp = libvlc_media_player_new_from_media( m_vlcMedia, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) )
        return ;
    libvlc_media_release( m_vlcMedia );
    libvlc_media_player_play( m_vlcMp, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return ;
    sleep( 10 );
}

void Transcode::on_dialogButtonBox_accepted()
{
    if ( m_origVidPath == "" )
    {
        QMessageBox::warning( this,
                              tr( "Warning" ),
                              tr( "Choose a video to transcode" ) );
        return ;
    }
    QString path = QFileDialog::getOpenFileName( this,
                                                 tr( "Choose File to save" ),
                                                 m_origVidPath,
                                                 tr( "Video files (*.avi *.mkv *.ogg)" ) );
    if ( path == "" )
        return ;
    QString type;
    QString transCodeString = "";

    type = m_ui.profileSelector->itemData( m_ui.profileSelector->currentIndex() ).toString();
    if ( type ==  "high" )
    {
        transCodeString = "transcode{vcodec=h264,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=mp4a,ab=128,"
                          "channels=2,samplerate=44100}:std{access=file,mux=ts,dst=\"";
    }
    else if ( type ==  "medium" )
    {
        transCodeString = "transcode{vcodec=mp4v,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=mp4a,ab=128,"
                          "channels=2,samplerate=44100}:std{access=file,mux=mp4,dst=\"";
    }
    else if  ( type == "low" ) 
    {
        transCodeString = "transcode{vcodec=WMV2,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=wma,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dts=std{access=file,mux=asf,dst=\"";
    }
    transCodeString += path;
    transCodeString += "\"}";

    //TODO : instanciate VLC instance and launch transcode
    m_doTranscode(path, transCodeString);
    close();
}

void Transcode::on_dialogButtonBox_rejected()
{
    close();
}

void Transcode::on_addProfile_clicked()
{
    //TODO : show the profiles management widget;
}

void Transcode::on_editProfile_clicked()
{
    //TODO : show the profile management widget and load the selected profile
}

void Transcode::on_deleteProfile_clicked()
{
    //TODO : delete the selected profile
}


