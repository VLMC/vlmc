#ifndef VLCMEDIA_H
#define VLCMEDIA_H

#include "vlc/vlc.h"

#include <QString>
#include <QMutex>

#include "VLCpp.hpp"
#include "VLCException.h"
#include "VLCInstance.h"

#define VIDEOWIDTH 854
#define VIDEOHEIGHT 480

namespace LibVLCpp
{
    class   Media : public Internal<libvlc_media_t>
    {
    public:
        struct          DataCtx
        {
            ~DataCtx();
            QMutex*         mutex;
            Media*          media;
        };
        typedef void    (*lockCallback)(Media::DataCtx* dataCtx, void **pp_ret);
        typedef void    (*unlockCallback)(Media::DataCtx* dataCtx);

        Media(Instance* instance, const QString& filename);
        ~Media();
        void                addOption(const char* opt);
        void                setLockCallback(Media::lockCallback);
        void                setUnlockCallback(Media::unlockCallback);
        void                setDataCtx();
        void                outputInVmem();
        void                outputInWindow();
        void                setPixelBuffer(uchar* buffer);
        uchar*              getPixelBuffer();

    private:
        Exception           _ex;
        Instance&           _instance;
        DataCtx*            _dataCtx;
        uchar*              _pixelBuffer;
    };
}

#endif // VLCMEDIA_H
