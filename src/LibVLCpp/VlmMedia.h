#ifndef VLMMEDIA_H
#define VLMMEDIA_H

#include <QString>

namespace LibVLCpp
{
    class   VlmMedia
    {
    public:
        VlmMedia(const QString& filename);

        const QString&      getHash() const;
    private:
        QString     _hash;
    };
}

#endif // VLMMEDIA_H
