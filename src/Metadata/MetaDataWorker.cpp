/*****************************************************************************
 * MetaDataWorker.cpp: Implement the thread that will get the media informations
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
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

#include <QtDebug>
#include <QPainter>
#include <QLabel>
#include <QImage>
#include "vlmc.h"
#include "MetaDataWorker.h"
#include "Library.h"

#include <QThreadPool>
#include <QRunnable>

class Help : public QRunnable
{

    private:
        QList<int>*     m_audioValueList;
        QImage*         m_image;
        QPainter*       m_painter;
        QPainterPath    m_path;

    public:
        Help(QList<int>* audioValueList) : m_audioValueList( audioValueList )
        {
            m_image = new QImage( m_audioValueList->count(), 800, QImage::Format_RGB32);
            m_image->fill( 0 );
            m_painter = new QPainter( m_image );
            m_painter->setRenderHint( QPainter::Antialiasing, true );
            m_painter->setPen( QPen( QColor( 79, 106, 25 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin ) );
        }

        void    run()
        {
            for( int x = 0; x < m_audioValueList->count(); x++ )
            {
                int y = m_audioValueList->at(x);
                //qDebug() << y;
                m_path.lineTo( x, ( y / 30 ) - 700 );
            }
            m_painter->drawPath( m_path );
        }
};

MetaDataWorker::MetaDataWorker( LibVLCpp::MediaPlayer* mediaPlayer, Media* media, MetaDataWorker::MetaDataType type ) :
        m_mediaPlayer( mediaPlayer ),
        m_type( type ),
        m_media( media ),
        m_mediaIsPlaying( false),
        m_lengthHasChanged( false )
{
    m_audioValueList = new QList<int>();
}

MetaDataWorker::~MetaDataWorker()
{
    //if ( m_mediaPlayer->isPlaying() )
    //    m_mediaPlayer->stop();
}

void    MetaDataWorker::compute()
{
    if ( m_media->getFileType() == Media::Video )
    {
        if ( m_type == AudioSpectrum )
            computeAudioMetaData();
        else
            computeVideoMetaData();
    }
    else if ( m_media->getFileType() == Media::Image )
    {
        computeImageMetaData();
    }

    m_media->addConstantParam( ":vout=dummy" );
    m_mediaPlayer->setMedia( m_media->getVLCMedia() );
    connect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
    m_mediaPlayer->play();
    m_media->flushVolatileParameters();
}

void    MetaDataWorker::computeVideoMetaData()
{
    //Disabling audio for this specific use of the media
    m_media->addVolatileParam( ":no-audio", ":audio" );
    connect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
}

void    MetaDataWorker::computeImageMetaData()
{
    m_media->addVolatileParam( ":access=fake", ":access=''" );
    m_media->addVolatileParam( ":fake-duration=10000", ":fake-duration=''" );
    //There can't be a length for an image file, so we don't have to wait for it to be updated.
    m_lengthHasChanged = true;
}

void    MetaDataWorker::computeAudioMetaData()
{
    m_media->getVLCMedia()->addOption( ":no-sout-video" );
    m_media->getVLCMedia()->addOption( ":sout=#transcode{}:smem" );
    m_media->getVLCMedia()->setAudioDataCtx( this );
    m_media->getVLCMedia()->setAudioLockCallback( reinterpret_cast<void*>( lock ) );
    m_media->getVLCMedia()->setAudioUnlockCallback( reinterpret_cast<void*>( unlock ) );
    m_media->getVLCMedia()->addOption( ":sout-transcode-acodec=s16l" );
    m_media->getVLCMedia()->addOption( ":no-sout-smem-time-sync" );
    connect( m_mediaPlayer, SIGNAL( endReached() ), this, SLOT( generateAudioSpectrum() ) );
}

void    MetaDataWorker::getMetaData()
{
    m_mediaIsPlaying = false;
    m_lengthHasChanged = false;

    //In order to wait for the VOUT to be ready:
    //Until we have a way of knowing when it is, both getWidth and getHeight method
    //will trigger exception... so we shut it up.
    LibVLCpp::Exception::setErrorCallback( LibVLCpp::Exception::silentExceptionHandler );
    while ( m_mediaPlayer->hasVout() == false )
    {
        SleepMS( 1 ); //Ugly isn't it :)
    }
    LibVLCpp::Exception::setErrorCallback( NULL );

    if ( m_type == MetaData )
    {
        m_media->setLength( m_mediaPlayer->getLength() );
        m_media->setWidth( m_mediaPlayer->getWidth() );
        m_media->setHeight( m_mediaPlayer->getHeight() );
        m_media->setFps( m_mediaPlayer->getFps() );
        if ( m_media->getFps() == .0f )
        {
            qWarning() << "Invalid FPS for media:" << m_media->getFileInfo()->absoluteFilePath();
            m_media->setFps( FPS );
        }
        m_media->setNbFrames( (m_media->getLengthMS() / 1000) * m_media->getFps() );
//        connect( m_mediaPlayer, SIGNAL( stopped () ), this, SLOT( mediaPlayerStopped() ), Qt::QueuedConnection );
        m_mediaPlayer->stop();
        emit mediaPlayerIdle( m_mediaPlayer );
        m_media->emitMetaDataComputed( true );
        delete this;
        return;
    }
    else if ( m_type == Snapshot )
    {
        //Setting time for snapshot :
        if ( m_media->getFileType() == Media::Video )
        {
            connect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
            m_mediaPlayer->setTime( m_mediaPlayer->getLength() / 3 );
        }
        else
            connect( this, SIGNAL( snapshotRequested() ), this, SLOT( renderSnapshot() ) );
    }
}

void    MetaDataWorker::renderSnapshot()
{
    if ( m_media->getFileType() == Media::Video )
        disconnect( m_mediaPlayer, SIGNAL( positionChanged() ), this, SLOT( renderSnapshot() ) );
    else
        disconnect( this, SIGNAL( snapshotRequested() ), this, SLOT( renderSnapshot() ) );
    QTemporaryFile tmp;
    tmp.setAutoRemove( false );
    tmp.open();
    m_tmpSnapshotFilename = tmp.fileName();

    connect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ), Qt::QueuedConnection );

    //The slot should be triggered in this methode
    m_mediaPlayer->takeSnapshot( m_tmpSnapshotFilename.toStdString().c_str()
                                 , 0, 0 );
    //Snapshot slot should has been called (but maybe not in next version...)
}

void    MetaDataWorker::setSnapshot()
{
    QPixmap* pixmap = new QPixmap( m_tmpSnapshotFilename );
    if ( pixmap->isNull() )
        delete pixmap;
    else
        m_media->setSnapshot( pixmap );
    //TODO : we shouldn't have to do this... patch vlc to get a memory snapshot.
    QFile   tmp( m_tmpSnapshotFilename );
    tmp.remove();

    disconnect( m_mediaPlayer, SIGNAL( snapshotTaken() ), this, SLOT( setSnapshot() ) );

    //CHECKME:
    //This is synchrone, but it may become asynchrone in the future...
//    connect( m_mediaPlayer, SIGNAL( stopped () ), this, SLOT( mediaPlayerStopped() ), Qt::QueuedConnection );
    m_mediaPlayer->stop();
    emit mediaPlayerIdle( m_mediaPlayer );

    if ( m_type == Snapshot )
        m_media->emitSnapshotComputed();
    else
        m_media->emitMetaDataComputed( true );

    delete this;
}

void    MetaDataWorker::mediaPlayerStopped()
{
    disconnect( m_mediaPlayer, SIGNAL( stopped() ), this, SLOT( mediaPlayerStopped() ) );
    emit mediaPlayerIdle( m_mediaPlayer );
    if ( m_type == Snapshot )
        m_media->emitSnapshotComputed();
    else
        m_media->emitMetaDataComputed( true );
    delete this;
}

void    MetaDataWorker::entrypointLengthChanged()
{
    disconnect( m_mediaPlayer, SIGNAL( lengthChanged() ), this, SLOT( entrypointLengthChanged() ) );
    m_lengthHasChanged = true;
    if ( m_mediaIsPlaying == true )
        getMetaData();
}

void    MetaDataWorker::entrypointPlaying()
{
    disconnect( m_mediaPlayer, SIGNAL( playing() ), this, SLOT( entrypointPlaying() ) );
    m_mediaIsPlaying = true;
    if ( m_lengthHasChanged == true )
        getMetaData();
}

void        MetaDataWorker::lock( MetaDataWorker* metaDataWorker, uint8_t** pcm_buffer , unsigned int size )
{
    if ( metaDataWorker->m_audioBuffer == NULL )
        metaDataWorker->m_audioBuffer = new unsigned char[size];
    *pcm_buffer = metaDataWorker->m_audioBuffer;
}

void        MetaDataWorker::unlock( MetaDataWorker* metaDataWorker, uint8_t* pcm_buffer,
                                      unsigned int channels, unsigned int rate,
                                      unsigned int nb_samples, unsigned int bits_per_sample,
                                      unsigned int size, int pts )
{
    Q_UNUSED( rate );
    Q_UNUSED( size );
    Q_UNUSED( pts );

    int bytePerChannelPerSample = bits_per_sample / 8;
    int bytePerSample = bytePerChannelPerSample * channels;

    int leftAverage = 0;
    int rightAverage = 0;

    int it = 0;
    for ( int i = 0; i < nb_samples; i++)
    {
        int left = 0;
        int right = 0;
        for ( int u = 0 ; u < bytePerChannelPerSample; u++, it++ )
        {
            int increment = 0;
            if ( channels == 2 )
                increment = bytePerChannelPerSample;
            left <<= 8;
            left += pcm_buffer[ it ];
            right <<= 8;
            right += pcm_buffer[ it + increment ];
        }
        leftAverage += left;
        rightAverage += right;
    }
    leftAverage /= nb_samples;
    metaDataWorker->addAudioValue( leftAverage );
}

void    MetaDataWorker::generateAudioSpectrum()
{
    if ( m_mediaIsPlaying == true )
    {
        qDebug() << "generateAudioSpectrum";
        disconnect( m_mediaPlayer, SIGNAL(endReached()), this, SLOT(generateAudioSpectrum()));
        m_mediaPlayer->stop();
        emit mediaPlayerIdle( m_mediaPlayer );

        m_media->emitMetaDataComputed( true );

        //generateAudioPixmap();
        Help* h = new Help(m_audioValueList);
        QThreadPool::globalInstance()->start(h);
        delete this;
    }
}

void    MetaDataWorker::addAudioValue( int value )
{
    m_audioValueList->append( value );
}

void    MetaDataWorker::generateAudioPixmap()
{
    m_audioDebugWidget = new QLabel();
    m_audioDebugWidget->setFixedSize( m_audioValueList->count(), 800 );
    m_audioDebugWidget->show();

    //QImage* image = new QImage( 300, 300, QImage::Format_RGB32);
    QPixmap image(m_audioValueList->count(), 800);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen( QPen( QColor( 79, 106, 25 ), 1, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin ) );

    QPainterPath path;
    for( int x = 0; x < m_audioValueList->count(); x++ )
    {
//        qDebug() << (m_audioValueList[x] / 30) - 700;
        int y = m_audioValueList->at(x);
        path.lineTo( x, ( y / 30) - 700);
    }

    painter.drawPath(path);
    m_audioDebugWidget->setPixmap(image);
}
