#ifndef AUDIOSPECTRUMDRAWER_H
#define AUDIOSPECTRUMDRAWER_H

#include <QList>
#include <QPainter>
#include <QRunnable>

class AudioSpectrumDrawer : public QRunnable
{
    private:
        QPainter*       m_painter;
        int             m_height;
        int             m_width;
        QList<int>*     m_audioValueList;
        QPainterPath    m_path;

    public:
        AudioSpectrumDrawer( QPainter* painter, int height, int width, QList<int>* audioValueList );
        void    run();
};

#endif // AUDIOSPECTRUMDRAWER_H
