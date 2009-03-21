#ifndef VLCPP_H
#define VLCPP_H

#include <cassert>
#include <stdlib.h>

namespace LibVLCpp
{
    template <typename T>
    class   Internal
    {
        public:
            typedef     T*      internalPtr;
            T*                  getInternalPtr()
            {
                assert(this->_internalPtr != NULL);
                return this->_internalPtr;
            }
            operator T*() {return this->_internalPtr;}
        protected:
            Internal() : _internalPtr(NULL) {}

            T*                  _internalPtr;
    };
}

#endif // VLCPP_H
