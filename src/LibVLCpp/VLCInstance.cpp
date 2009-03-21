#include <cassert>
#include "VLCInstance.h"
#include <QtDebug>

using namespace LibVLCpp;

Instance::Instance(int argc, const char** argv)
{
    this->_internalPtr = libvlc_new(argc, argv, this->_ex);
    this->_ex.checkThrow();
}
