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
    const QPixmap*  getThumbnail() const;
    QString         title() const;
    const QUuid&    uuid() const;

protected:
    void changeEvent( QEvent *e );

private:
    Ui::MediaCellView   *m_ui;
    const QUuid         m_uuid;
    QPoint              m_dragStartPos;

protected:
    void        mouseDoubleClickEvent( QMouseEvent* );
    void        mousePressEvent( QMouseEvent* );
    void        mouseMoveEvent( QMouseEvent* );

signals:
    void        cellSelected( const QUuid& uuid );


};

#endif // MEDIACELLVIEW_H
