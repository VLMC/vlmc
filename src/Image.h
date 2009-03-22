#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <QImage>

class   Image
{
public:
    Image(int width, int height)
    {
        this->_pixelsData = new uchar[width * height * 4];
        this->_image = new QImage(this->_pixelsData, width, height, width * 4, QImage::Format_ARGB32);
    }
    ~Image()
    {
        delete this->_image;
        delete this->_pixelsData;
    }
    uchar*      getBuffer()
    {
        return this->_pixelsData;
    }

    QImage&     getImage()
    {
        return *(this->_image);
    }

private:
    QImage*     _image;
    uchar*      _pixelsData;
};

#endif // IMAGE_HPP
