#ifndef IMPORTCONTROLLER_H
#define IMPORTCONTROLLER_H

#include <QDialog>
#include <QDirModel>
#include <QFileSystemWatcher>

#include "Media.h"
#include "Clip.h"
#include "ImportModel.h"
#include "PreviewWidget.h"
#include "StackViewController.h"
#include "TagWidget.h"
#include "ImportMediaListController.h"

namespace Ui {
    class ImportController;
}

class ImportController : public QDialog {
    Q_OBJECT

public:
    ImportController(QWidget *parent = 0);
    ~ImportController();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ImportController*       m_ui;
    PreviewWidget*              m_preview;
    ImportModel*                m_model;
    StackViewController*        m_stackNav;
    TagWidget*                  m_tag;
    ImportMediaListController*  m_mediaListController;
    QDirModel*                  m_filesModel;
    QFileSystemWatcher*         m_fsWatcher;
    QString                     m_currentlyWatchedDir;

public slots:
    void        newMediaLoaded( Media* media );
    void        updateMediaRequested( Media* media );
    void        accept();

private slots:
    void forwardButtonClicked();
    void treeViewClicked( const QModelIndex& index );
    void treeViewDoubleClicked( const QModelIndex& index);
};

#endif // IMPORTCONTROLLER_H
