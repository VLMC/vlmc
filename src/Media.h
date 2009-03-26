#ifndef MEDIA_H
#define MEDIA_H

#include <QString>
#include <QImage>

#include "VLCMedia.h"
#include "VLCInstance.h"
#include "VLCMediaPlayer.h"

#include "Image.h"

class       Media
{
public:
    Media(const QString& mrl);
    ~Media();

    static void             lock(LibVLCpp::Media::DataCtx* dataCtx, void **pp_ret);
    static void             unlock(LibVLCpp::Media::DataCtx* dataCtx);
    QImage*                 takeSnapshot(unsigned int width, unsigned int heigth);

    QImage&                 getImage();
    //FIXME into bool
    int                     isPlaying();
    int                     isSeekable();
    qint64                  getLength();


    //FIXME
private: public:
    LibVLCpp::Media*            _vlcMedia;
    LibVLCpp::MediaPlayer*      _vlcMediaPlayer;
    LibVLCpp::Instance*         _instance;
    QString                     _mrl;

    QImage*                     _snapshot;

    uchar*                      _pixelBuffer;
    QImage*                     _image;


};

#endif // MEDIA_H
