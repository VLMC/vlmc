#include <QtDebug>
#include "Media.h"

Media::Media(const QString& mrl) : _mrl(mrl), _snapshot(NULL)
{
    this->_instance = new LibVLCpp::Instance(0, NULL);
//    qDebug() << "Built instance";
    this->_vlcMedia = new LibVLCpp::Media(this->_instance, this->_mrl);
//    qDebug() << "Built Media";

//    qDebug() << "Built mediaPlayer";

    this->_vlcMedia->outputInVmem();
    this->_vlcMedia->setLockCallback(Media::lock);
    this->_vlcMedia->setUnlockCallback(Media::unlock);

    char    width[64], height[64], chroma[64], pitch[64];
    sprintf(width, ":vmem-width=%i", VIDEOWIDTH);
    sprintf(height, ":vmem-height=%i", VIDEOHEIGHT);
    sprintf(chroma, ":vmem-chroma=%s", "RV32");
    sprintf(pitch, ":vmem-pitch=%i", VIDEOWIDTH * 4);

    this->_vlcMedia->addOption(width);
    this->_vlcMedia->addOption(height);
    this->_vlcMedia->addOption(chroma);
    this->_vlcMedia->addOption(pitch);

    this->_pixelBuffer = new uchar[VIDEOHEIGHT * VIDEOWIDTH * 4];
    this->_image = new QImage(this->_pixelBuffer, VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH * 4, QImage::Format_RGB32);
    this->_image->fill(0);

    //Once we got the pixel buffer up and running, we can put it in the "render context"
    this->_vlcMedia->setPixelBuffer(this->_pixelBuffer);
    this->_vlcMedia->setDataCtx();

    //And now we play the media
    this->_vlcMediaPlayer = new LibVLCpp::MediaPlayer(this->_vlcMedia);
}

Media::~Media()
{
    delete this->_image;
    delete this->_pixelBuffer;
    delete this->_vlcMedia;
    delete this->_vlcMediaPlayer;
    delete this->_instance;
}

void        Media::lock(LibVLCpp::Media::DataCtx* ctx, void **renderPtr)
{
    ctx->mutex->lock();
    *renderPtr = ctx->media->getPixelBuffer();
}

void        Media::unlock(LibVLCpp::Media::DataCtx* ctx)
{
//    QImage(ctx->media->getPixelBuffer(), VIDEOWIDTH, VIDEOHEIGHT, QImage::Format_RGB32).save("/home/chouquette/Desktop/testRender.png");
    qDebug() << "frame just rendered";
    ctx->mutex->unlock();
}

QImage*      Media::takeSnapshot(unsigned int width, unsigned int height)
{
    if ( this->_snapshot == NULL )
    {
        this->_snapshot = new QImage(*this->_image);

        this->_vlcMediaPlayer->setTime(this->_vlcMediaPlayer->getLength() / 2);
        this->_vlcMediaPlayer->setTime(0);
    }
    return this->_snapshot;
}

int         Media::isPlaying()
{
    return this->_vlcMediaPlayer->isPlaying();
}

int         Media::isSeekable()
{
    return this->_vlcMediaPlayer->isSeekable();
}
