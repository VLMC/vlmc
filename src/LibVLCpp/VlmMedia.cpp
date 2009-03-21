#include <QtDebug>
#include <QCryptographicHash>

#include "vlc/vlc.h"

#include "VlmMedia.h"


using namespace LibVLCpp;

VlmMedia::VlmMedia(Instance& instance, const QString& filename) : _instance(instance)
{
    QByteArray  hash = QCryptographicHash::hash(filename.toAscii(), QCryptographicHash::Md5);
    //To have a printable value :
    this->_hash = hash.toHex();
}


const QString&  VlmMedia::getHash() const
{
    return this->_hash;
}

void    VlmMedia::play()
{
    libvlc_vlm_play_media(this->_instance, this->_hash.toLocal8Bit(), this->_ex);
    this->_ex.checkThrow();
}

int     VlmMedia::getLength()
{
    int length = libvlc_vlm_get_media_instance_length(this->_instance, this->_hash.toLocal8Bit(), 0, this->_ex);
    this->_ex.checkThrow();
    return length;
}
