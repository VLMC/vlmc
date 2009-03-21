#ifndef VLMMEDIA_H
#define VLMMEDIA_H

#include <QString>

#include "VLCInstance.h"
#include "VLCException.h"

namespace LibVLCpp
{
    class   VlmMedia
    {
    public:
        VlmMedia(Instance& inst, const QString& filename);

        const QString&      getHash() const;
        int                 getLength();
        void                play();
    private:
        Instance&   _instance;
        Exception   _ex;
        QString     _hash;
    };
}

#endif // VLMMEDIA_H
