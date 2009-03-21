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
            uchar*          pixelBuffer;
            QMutex*         mutex;
        };
        typedef void    (*lockCallback)(Media::DataCtx* dataCtx, void **pp_ret);
        typedef void    (*unlockCallback)(Media::DataCtx* dataCtx);

        Media(Instance* instance, const QString& filename);
        ~Media();
        void                addOption(const char* opt);
        void                setLockCallback(Media::lockCallback);
        void                setUnlockCallback(Media::unlockCallback);
        void                setDataCtx(Media::DataCtx* dataCtx);
        void                outputInVmem();
        void                outputInWindow();

        static DataCtx*     buildDataCtx();
    private:
        Exception           _ex;
        Instance&           _instance;
    };
}

#endif // VLCMEDIA_H
