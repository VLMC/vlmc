#ifndef MEDIALIBRARYWIDGET_H
#define MEDIALIBRARYWIDGET_H

#include <QWidget>

#include "StackViewController.h"
#include "MediaListViewController.h"

class MediaLibraryWidget : public QWidget
{
    Q_OBJECT

public:
    MediaLibraryWidget( QWidget* parent = 0 );
    virtual ~MediaLibraryWidget();

private:
    StackViewController*    m_nav;
};

#endif // MEDIALIBRARYWIDGET_H
