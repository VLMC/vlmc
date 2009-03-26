#ifndef VLM_H
#define VLM_H

#include <QString>
#include <QHash>

#include "VLCException.h"
#include "VLCInstance.h"
#include "VlmMedia.h"

namespace       LibVLCpp
{
    class   VlmManager
    {
    public:
        VlmManager( Instance* instance );

        VlmMedia*       addMedia( const QString& filename, const char* const* argv, int argc );
        VlmMedia*       getMedia( const QString& hash );
    private:
        QHash< QString, VlmMedia* >     m_hashTable;
        Instance&                       m_instance;
        Exception                       m_ex;
    };
}

#endif // VLM_H
