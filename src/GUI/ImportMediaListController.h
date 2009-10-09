#ifndef IMPORTMEDIALISTCONTROLLER_H
#define IMPORTMEDIALISTCONTROLLER_H

#include "StackViewController.h"
#include "ListViewController.h"
#include "Media.h"
#include "ImportMediaCellView.h"

class ImportMediaListController : public ListViewController
{
    Q_OBJECT

    public:
        ImportMediaListController( StackViewController* nav );
        ~ImportMediaListController();
        void    addMedia( Media* media );

        const QHash<QUuid, ImportMediaCellView*>* getMediaCellList() const { return m_mediaCellList; }
        ImportMediaCellView* getCell( QUuid uuid ) const;

    private:
        StackViewController*            m_nav;
        QHash<QUuid, ImportMediaCellView*>*   m_mediaCellList;

    public slots:
        void    metaDataComputed( Media* media );
};

#endif // IMPORTMEDIALISTCONTROLLER_H
