#include <QCryptographicHash>
#include "VlmMedia.h"

using namespace LibVLCpp;

VlmMedia::VlmMedia(const QString& filename)
{
    QByteArray  hash = QCryptographicHash::hash(filename.toAscii(), QCryptographicHash::Md5);
    this->_hash = hash;
}


const QString&  VlmMedia::getHash() const
{
    return this->_hash;
}
