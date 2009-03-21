#ifndef VLCINSTANCE_H
#define VLCINSTANCE_H

#include "vlc/vlc.h"
#include "VLCpp.hpp"
#include "VLCException.h"

namespace LibVLCpp
{
    class   Instance : public Internal<libvlc_instance_t>
    {
    public:
        Instance(int argc, const char** argv);
    private:
        Exception       _ex;
    };
}

#endif // VLCINSTANCE_H
