#ifndef IMPORTMEDIALISTCONTROLLER_H
#define IMPORTMEDIALISTCONTROLLER_H

#include "StackViewController.h"
#include "ListViewController.h"
#include "Media.h"
#include "Clip.h"
#include "ImportMediaCellView.h"

class ImportMediaListController : public ListViewController
{
    Q_OBJECT

    public:
        ImportMediaListController( StackViewController* nav );
        ~ImportMediaListController();
        void    addMedia( Media* media );
        void    removeMedia( const QUuid& uuid );
        void    addClip( Clip* media );
        void    removeClip( const QUuid& uuid );
        void    cleanAll();
        void    addClipsFromMedia( Media* media );

        const QHash<QUuid, ImportMediaCellView*>* getMediaCellList() const { return m_mediaCellList; }
        ImportMediaCellView* getCell( QUuid uuid ) const;

    private:
        StackViewController*            m_nav;
        QHash<QUuid, ImportMediaCellView*>*   m_mediaCellList;

    public slots:
        void    metaDataComputed( Media* media );
};

#endif // IMPORTMEDIALISTCONTROLLER_H
