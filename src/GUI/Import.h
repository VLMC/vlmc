#ifndef IMPORT_H
#define IMPORT_H

#include <QDialog>
#include "PreviewWidget.h"
#include "ImportBrowser.h"
#include "Clip.h"
#include "Media.h"
#include "MetaDataWorker.h"

namespace Ui
{
    class Import;
}

class Import : public QDialog
{
    Q_OBJECT
public:
    Import( QWidget *parent = 0 );
    ~Import();

protected:
    void changeEvent( QEvent *e );

private:
    Ui::Import*         m_ui;
    PreviewWidget*      m_previewWidget;
    ImportBrowser*      m_importBrowser;
    Media*              m_currentMedia;
    Clip*               m_currentClip;
    MetaDataWorker*     m_metaDataWorker;

private slots:
    void    accept();
    void    getMetaData( QFileInfo fileInfos );
    void    setUIMetaData();

signals:
    void    mediaSelected( Clip* clip );
};

#endif // IMPORT_H
