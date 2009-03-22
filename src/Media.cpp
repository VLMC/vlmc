#include "Media.h"

Media::Media(const QString& mrl) : _mrl(mrl), _snapshot(NULL)
{
    this->_instance = new LibVLCpp::Instance(0, NULL);
    this->_vlcMedia = new LibVLCpp::Media(this->_instance, this->_mrl);
    this->_vlcMediaPlayer = new LibVLCpp::MediaPlayer(this->_vlcMedia);

    this->_vlcMedia->outputInVmem();
    this->_vlcMedia->setLockCallback(Media::lock);
    this->_vlcMedia->setUnlockCallback(Media::unlock);

    this->_pixelBuffer = new uchar[VIDEOHEIGHT * VIDEOWIDTH * 4];
    this->_image = new QImage(this->_pixelBuffer, VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH * 4, QImage::Format_ARGB32);
    this->_image->fill(0);

    this->_vlcMedia->setPixelBuffer(this->_pixelBuffer);
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
    ctx->mutex->unlock();
}

Image*      Media::takeSnapshot(unsigned int width, unsigned int height)
{
    if ( this->_snapshot == NULL )
    {
        this->_snapshot = new Image(width, height);
        this->_vlcMediaPlayer->setTime(this->_vlcMediaPlayer->getLength() / 2);
        this->_vlcMediaPlayer->takeSnapshot(this->_snapshot->getBuffer(), width, height);
        this->_vlcMediaPlayer->setTime(0);
    }
    return this->_snapshot;
}
