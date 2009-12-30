#ifndef MEDIALISTVIEWCONTROLLER_H
#define MEDIALISTVIEWCONTROLLER_H

#include "StackViewController.h"
#include "ListViewController.h"
#include "MediaCellView.h"
#include "ClipListViewController.h"
#include "Library.h"
#include "Media.h"

class MediaListViewController : public ListViewController
{
    Q_OBJECT

public:
    MediaListViewController( StackViewController* nav );
    virtual ~MediaListViewController();

private:
    void                    updateClipCount();
    StackViewController*    m_nav;
    QUuid                   m_currentUuid;
    QHash<QUuid, QWidget*>* m_cells;
    ClipListViewController* m_clipsListView;
    QUuid                   m_lastUuidClipListAsked;

public slots:
    void        newMediaLoaded( Media* );
    void        cellSelection( const QUuid& uuid );
    void        mediaDeletion( const QUuid& uuid );
    void        mediaRemoved( const QUuid& uuid );
    void        updateCell( Media* media );
    void        showClipList( const QUuid& uuid );
    void        newClipAdded( Clip* clip );

private slots:
    void    restoreContext();
signals:
    void        mediaSelected( Media* media );
    void        mediaDeleted( const QUuid& uuid );

};
#endif // MEDIALISTVIEWCONTROLLER_H
