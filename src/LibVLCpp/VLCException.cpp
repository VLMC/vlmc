#include <QtDebug>
#include "VLCException.h"

using namespace LibVLCpp;

Exception::errorCallback        Exception::_errorCallback = NULL;
void*                           Exception::_datas = NULL;

Exception::Exception()
{
    this->_internalPtr = new libvlc_exception_t;
    libvlc_exception_init(this->_internalPtr);
}

Exception::~Exception()
{
    this->clear();
    delete this->_internalPtr;
}

void        Exception::setErrorCallback(Exception::errorCallback handler, void* datas)
{
    Exception::_datas = datas;
    Exception::_errorCallback = handler;
}

const char* Exception::getErrorText() const
{
    return libvlc_exception_get_message(this->_internalPtr);
}

void        Exception::clear()
{
    libvlc_exception_clear(this->_internalPtr);
}

int         Exception::raised() const
{
    return libvlc_exception_raised(this->_internalPtr);
}

void        Exception::checkThrow()
{
    if (this->raised() == Exception::Raised)
    {
        if (Exception::_errorCallback != NULL)
            Exception::_errorCallback(this->getErrorText(), Exception::_datas);
        else
            qWarning() << "An exception was raised, but no error handler is set.\nError message is: " << this->getErrorText();
        this->clear();
    }
}
