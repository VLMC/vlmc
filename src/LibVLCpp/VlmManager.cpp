#include "VlmManager.h"
#include <QString>

using namespace LibVLCpp;

VlmManager::VlmManager( Instance* instance ) : m_instance( *instance )
{
}

VlmMedia*       VlmManager::addMedia( const QString& filename, const char* const* argv, int argc )
{  
    VlmMedia* media = new VlmMedia( m_instance, filename );

    libvlc_vlm_add_broadcast( m_instance, media->getHash().toLocal8Bit(), filename.toLocal8Bit(),
                             "#duplicate{dst=display{vmem}}",
                             argc, argv, true, false, m_ex );
    m_ex.checkThrow();
    m_hashTable[ media->getHash() ] = media;
    return media;
}

VlmMedia*       VlmManager::getMedia( const QString& hash )
{
    if ( m_hashTable.contains( hash ) )
        return m_hashTable[ hash ];
    return NULL;
}
