#ifndef VLCEXCEPTION_H
#define VLCEXCEPTION_H

#include "vlc/vlc.h"
#include "VLCpp.hpp"

namespace LibVLCpp
{
    class   Exception : public Internal<libvlc_exception_t>
    {
    public:
        Exception();
        ~Exception();

        const char*             getErrorText() const;
        void                    clear();
        int                     raised() const;
        void                    checkThrow();

        static const int        Raised = 1;
        static const int        NotRaised = 0;

        //error handling part :
        //TODO: have a private error handling in which we could fall back for some specific cases
        typedef void            (*errorCallback)(const char* msg, void* data);
        static  void            setErrorCallback(errorCallback, void* data);

    private:
        static  errorCallback   _errorCallback;
        static  void*           _datas;
    };
}

#endif // VLCEXCEPTION_H
