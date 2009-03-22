#include <cassert>
#include "VLCMedia.h"

using namespace LibVLCpp;

Media::Media(Instance* instance, const QString& filename) : _instance(*instance), _pixelBuffer(NULL)
{
    this->_internalPtr = libvlc_media_new(this->_instance, filename.toLocal8Bit(), this->_ex);
    this->_ex.checkThrow();
//    this->_pixelBuffer = new uchar[VIDEOHEIGHT * VIDEOWIDTH * 4];
//    this->_dataCtx = this->buildDataCtx();
}

Media::~Media()
{
    libvlc_media_release(this->_internalPtr);
    delete[] this->_pixelBuffer;
    delete this->_dataCtx;
}

Media::DataCtx*         Media::buildDataCtx()
{
    Media::DataCtx* dataCtx = new Media::DataCtx;
    dataCtx->mutex = new QMutex();
    dataCtx->media = this;
    return dataCtx;
}

void                    Media::addOption(const char* opt)
{
    libvlc_media_add_option(this->_internalPtr, opt, this->_ex);
    this->_ex.checkThrow();
}

Media::DataCtx::~DataCtx()
{
    delete this->mutex;
}

void                    Media::setLockCallback(Media::lockCallback callback)
{
    char    param[64];
    sprintf(param, ":vmem-lock=%lld", (long long int)(intptr_t)callback);
    this->addOption(param);
}

void                    Media::setUnlockCallback(Media::unlockCallback callback)
{
    char    param[64];
    sprintf(param, ":vmem-unlock=%lld", (long long int)(intptr_t)callback);
    this->addOption(param);
}

void                    Media::setDataCtx(Media::DataCtx* dataCtx)
{
    char    param[64];
    sprintf(param, ":vmem-data=%lld", (long long int)(intptr_t)dataCtx);
    this->addOption(param);
}

void                    Media::outputInVmem()
{
    this->addOption(":vout=vmem");
}

void                    Media::outputInWindow()
{
//    this->addOption();
}

void                    Media::setPixelBuffer(uchar* buffer)
{
    this->_pixelBuffer = buffer;
}

uchar*                  Media::getPixelBuffer()
{
    return this->_pixelBuffer;
}

