#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QtGui/QDialog>
#include <QDragEnterEvent>

#include "InputMedia.h"

namespace Ui {
    class PreviewWidget;
}

class PreviewWidget : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( PreviewWidget )
public:
    explicit PreviewWidget( QWidget *parent = 0 );
    virtual ~PreviewWidget();

protected:
    virtual void    changeEvent( QEvent *e );
    virtual void    dragEnterEvent( QDragEnterEvent* event );
    virtual void    dropEvent( QDropEvent* event );

private:
    Ui::PreviewWidget*      m_ui;
    InputMedia*             m_currentMedia;
    LibVLCpp::Instance*     m_currentInstance;
};

#endif // PREVIEWWIDGET_H
