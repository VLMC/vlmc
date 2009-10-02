#ifndef MEDIACELLVIEW_H
#define MEDIACELLVIEW_H

#include <QWidget>
#include <QUuid>
#include <QMouseEvent>

namespace Ui
{
    class MediaCellView;
}

class MediaCellView : public QWidget
{
    Q_OBJECT

public:
    MediaCellView( const QUuid& uuid, QWidget *parent = 0 );
    ~MediaCellView();

    void            setTitle( const QString& title );
    void            setThumbnail( const QPixmap& pixmap );
    QString         title() const;
    const QUuid&    uuid() const;

protected:
    void changeEvent( QEvent *e );

private:
    Ui::MediaCellView   *m_ui;
    const QUuid         m_uuid;

protected:
    void        mouseDoubleClickEvent( QMouseEvent* );
};

#endif // MEDIACELLVIEW_H
