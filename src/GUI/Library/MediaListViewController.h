#ifndef MEDIALISTVIEWCONTROLLER_H
#define MEDIALISTVIEWCONTROLLER_H

#include "StackViewController.h"
#include "ListViewController.h"
#include "MediaCellView.h"
#include "Library.h"
#include "Media.h"

class MediaListViewController : public ListViewController
{
    Q_OBJECT

public:
    MediaListViewController( StackViewController* nav );
    virtual ~MediaListViewController();

private:
    StackViewController*    m_nav;
    QUuid                   m_currentUuid;
    QHash<QUuid, QWidget*>* m_cells;

public slots:
    void        newMediaLoaded( Media* );
    void        cellSelection( const QUuid& uuid );
signals:
    void        mediaSelected( Media* media );
};
#endif // MEDIALISTVIEWCONTROLLER_H
