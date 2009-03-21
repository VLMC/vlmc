#include "VlmManager.h"
#include <QString>

using namespace LibVLCpp;

VlmManager::VlmManager(Instance* instance) : _instance(*instance)
{
}

VlmMedia*       VlmManager::addMedia(const QString& filename, const char* const* argv, int argc)
{  
    VlmMedia* media = new VlmMedia(this->_instance, filename);

    libvlc_vlm_add_broadcast(this->_instance, media->getHash().toLocal8Bit(), filename.toLocal8Bit(),
                             "#duplicate{dst=display{vmem}}",
                             argc, argv, true, false, this->_ex);
    this->_ex.checkThrow();
    this->_hashTable[media->getHash()] = media;
    return media;
}

VlmMedia*       VlmManager::getMedia(const QString& hash)
{
    if ( this->_hashTable.contains(hash) == true )
        return this->_hashTable[hash];
    return NULL;
}
