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
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include "Transcode.h"

static bool catchVLCException( libvlc_exception_t *ex )
{
    if ( libvlc_exception_raised( ex ) )
    {
        qDebug() << libvlc_errmsg();
        libvlc_exception_clear( ex );
        return (true);
    }
    return (false);
}

Transcode *Transcode::m_instance = NULL;

Transcode *Transcode::instance( QWidget *parent )
{
    if ( !Transcode::m_instance )
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
    : QDialog( parent ), m_running( false )
{
    m_ui.setupUi( this );
    //TODO : load every known profiles
    m_ui.profileSelector->addItem( tr( "H264 + AAC" ), "h264" );
    m_ui.profileSelector->addItem( tr( "Dirac + AAC" ), "dirac" );
    m_ui.profileSelector->addItem( tr( "Theora + Vorbis" ), "thVorbis" );
    m_ui.profileSelector->addItem( tr( "Theora + Flac" ), "thFlac" );
    m_ui.profileSelector->addItem( tr( "Mpeg4 + AAC" ), "mp4" );
    m_ui.profileSelector->addItem( tr( "WMV + WMA" ), "wmv" );
    QObject::connect( qApp,
                      SIGNAL( aboutToQuit() ),
                      this,
                      SLOT( deleteLater() ) );
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

bool Transcode::m_doTranscode( const QString &transStr )
{
    QString sout( ":sout=" );
    sout += transStr;

    char const *vlc_argv[] = 
    {
        "-I", "dummy", 
        "--no-skip-frame",
    };

    int vlc_argc = sizeof( vlc_argv ) / sizeof( *vlc_argv );

    libvlc_exception_init( &m_vlcEx );
    
    m_libvlc = libvlc_new( vlc_argc, vlc_argv, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) )
        return false;
    
    m_vlcMedia = libvlc_media_new( m_libvlc, m_origVidPath.toLocal8Bit(), &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return false;

    libvlc_media_add_option(m_vlcMedia, sout.toStdString().c_str(), &m_vlcEx);
    if ( catchVLCException( &m_vlcEx ) ) 
        return false;

    m_vlcMp = libvlc_media_player_new_from_media( m_vlcMedia, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) )
        return false;

    m_vlcEM = libvlc_media_player_event_manager( m_vlcMp, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return false;

    libvlc_event_attach( m_vlcEM, libvlc_MediaPlayerPlaying, m_callback, this, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return false;
    libvlc_event_attach( m_vlcEM, libvlc_MediaPlayerEndReached, m_callback, this,  &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return false;

    libvlc_media_player_play( m_vlcMp, &m_vlcEx );
    if ( catchVLCException( &m_vlcEx ) ) 
        return false;
    m_running = true;
    m_initProgressDialog();

    return true;
}

void Transcode::m_initProgressDialog()
{
    m_progress = new QProgressDialog( tr( "Transcode in progress" ), "Cancel", 0, 0);
    QObject::connect( m_progress, SIGNAL( canceled() ),
                      this, SLOT( cancelTranscode() ) );
    m_timer = new QTimer( this );
    QObject::connect( m_timer, SIGNAL( timeout() ),
                      this, SLOT( calcTranscodePercentage() ) );
    m_timer->start( 500 );
}

void Transcode::m_releaseVLCRessources()
{
    libvlc_media_release( m_vlcMedia );
    libvlc_media_player_release( m_vlcMp );
    libvlc_release( m_libvlc ); 
}

void Transcode::m_callback(const libvlc_event_t *event, void *ptr)
{
    Transcode* self = reinterpret_cast<Transcode*>( ptr );
    switch ( event->type )
    {
    case libvlc_MediaPlayerEndReached:
        self->m_running = false;
        break;
    default:
        break;
    }
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
    QString path = QFileDialog::getSaveFileName( this,
                                                 tr( "Choose File to save" ),
                                                 m_origVidPath,
                                                 tr( "Video files (*.avi *.mkv *.ogg)" ) );
    if ( path == "" )
        return ;
    QString type;
    QString transCodeString = "";

    //TODO : Use a file or a qsettings, coz this is fugly
    type = m_ui.profileSelector->itemData( m_ui.profileSelector->currentIndex() ).toString();
    if ( type ==  "h264" )
    {
        transCodeString = "#transcode{vcodec=h264,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=mp3,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dst=std{access=file,mux=ts,dst=\"";
    }
    else if ( type ==  "mp4" )
    {
        transCodeString = "#transcode{vcodec=mp4v,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=mp4a,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dst={std{access=file,mux=mp4,dst=\"";
    }
    else if ( type == "dirac" )
    {
        transCodeString = "#transcode{vcodec=drac,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=mp4a,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dst=std{access=file,mux=ts,dst=\"";
    }
    else if ( type == "thVorbis" )
    {
        transCodeString = "#transcode{vcodec=theo,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=vorb,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dst=std{access=file,mux=ogg,dst=\"";
    }
    else if ( type == "thFlac" )
    {
        transCodeString = "transcode{vcodec=theo,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=flac,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dst=std{access=file,mux=ogg,dst=\"";
    }   
    else if ( type == "wmv" ) 
    {
        transCodeString = "#transcode{vcodec=WMV2,vb=800,scale=1,"
                          "fps=0,width=0,height=0,acodec=wma,ab=128,"
                          "channels=2,samplerate=44100}:duplicate{dst=std{access=file,mux=asf,dst=\"";
    }
    transCodeString += path;
    transCodeString += "\"}}";

    if ( !m_doTranscode( transCodeString ) )
        return ;
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

void Transcode::calcTranscodePercentage()
{
    m_progress->setValue( 10 );
    if ( !m_running )
    {
        m_timer->stop();
        m_releaseVLCRessources();
        delete m_progress;
        delete m_timer;
        m_running = false;
    }
}

void Transcode::cancelTranscode()
{
    m_timer->stop();
    libvlc_media_player_pause( m_vlcMp, &m_vlcEx );
    catchVLCException( &m_vlcEx );
    m_releaseVLCRessources();
    delete m_progress;
    delete m_timer;
    m_running = false;
}
