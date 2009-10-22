#include "ImportController.h"
#include "ui_ImportController.h"
#include "ClipRenderer.h"
#include "Library.h"

ImportController::ImportController(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ImportController)
{
    m_ui->setupUi(this);

    m_model = new ImportModel();
    m_preview = new PreviewWidget( new ClipRenderer, m_ui->previewContainer );
    m_stackNav = new StackViewController( m_ui->stackViewContainer, false );
    m_mediaListController = new ImportMediaListController( m_stackNav );
    m_tag = new TagWidget( m_ui->tagContainer, 6 );
    m_filesModel = new QDirModel();

    m_stackNav->pushViewController( m_mediaListController );

    QStringList filters;
    //Video
    filters << "*.mov" << "*.avi" << "*.mkv" << "*.mpg" << "*.mpeg" << "*.wmv" << "*.mp4";
    //Audio
    filters << "*.mp3" << "*.oga" << "*.flac" << "*.aac" << "*.wav";
    //Picture
    filters << "*.gif" << "*.png" << "*.jpg";

    m_filesModel->setFilter( QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot );
    m_filesModel->sort( 2, Qt::AscendingOrder );
    m_filesModel->sort( 0, Qt::AscendingOrder );
    m_filesModel->setNameFilters( filters );

    m_ui->treeView->setModel( m_filesModel );
    m_ui->treeView->setRootIndex( m_filesModel->index( QDir::rootPath() ) );
    m_ui->treeView->setCurrentIndex( m_filesModel->index( QDir::homePath() ) );
    m_ui->treeView->setExpanded( m_ui->treeView->currentIndex() , true );
    m_ui->treeView->setColumnHidden( 1, true );
    m_ui->treeView->setColumnHidden( 2, true );
    m_ui->treeView->setColumnHidden( 3, true );
    m_ui->forwardButton->setEnabled( false );

    m_fsWatcher = new QFileSystemWatcher();
    m_fsWatcher->addPath( QDir::homePath() );
    m_currentlyWatchedDir = QDir::homePath();

    connect( m_fsWatcher, SIGNAL( directoryChanged( QString ) ), m_filesModel, SLOT( refresh() ) );
    connect( m_ui->treeView, SIGNAL( clicked( QModelIndex ) ), this, SLOT( treeViewClicked( QModelIndex ) ) );
    connect( m_ui->treeView, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( treeViewDoubleClicked( QModelIndex ) ) );
    connect( m_ui->forwardButton, SIGNAL( clicked() ), this, SLOT( forwardButtonClicked() ) );

    connect( m_model, SIGNAL( newMediaLoaded( Media* ) ), this, SLOT( newMediaLoaded( Media* ) ) );
}

ImportController::~ImportController()
{
    delete m_ui;
    delete m_model;
    delete m_stackNav;
    delete m_tag;
}

void ImportController::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() ) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void        ImportController::newMediaLoaded( Media* media )
{
    qDebug() << media->getFileName();
    m_mediaListController->addMedia( media );
}

void        ImportController::updateMediaRequested( Media* media )
{
    Q_UNUSED( media );
}

void    ImportController::forwardButtonClicked()
{
    m_model->loadFile(m_filesModel->fileInfo( m_ui->treeView->selectionModel()->currentIndex() ).filePath());
}

void    ImportController::treeViewClicked( const QModelIndex& index )
{
    if ( m_filesModel->isDir( index ) )
    {
        m_fsWatcher->removePath( m_currentlyWatchedDir );
        m_currentlyWatchedDir = m_filesModel->filePath( index );
        m_fsWatcher->addPath( m_filesModel->filePath( index ) );
    }
    m_ui->forwardButton->setEnabled( true );
}

void    ImportController::treeViewDoubleClicked( const QModelIndex& index )
{
    if ( !m_filesModel->isDir( index ) )
        forwardButtonClicked();
}

void    ImportController::accept()
{
    QUuid id;
    foreach( id, m_model->getMedias()->keys() )
    {
        Media* media = m_model->getMedias()->value( id );
        Library::getInstance()->addMedia( media );
    }
    done( Accepted );
}
