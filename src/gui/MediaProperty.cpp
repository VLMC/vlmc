#include <QtDebug>
#include <QTime>
#include <QPushButton>
#include <QStringListModel>
#include <QInputDialog>

#include "MediaProperty.h"
#include "ui_MediaProperty.h"

MediaProperty::MediaProperty( Clip* clip, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::MediaProperty ),
    m_clip( clip )
{
    QTime   duration;
    duration = duration.addSecs( m_clip->getLengthSecond() );

    ui->setupUi(this);
    connect( this, SIGNAL( accepted() ), this, SLOT( deleteLater() ) );
    connect( this, SIGNAL( rejected() ), this, SLOT( deleteLater() ) );
    ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    ui->nameValueLabel->setText( m_clip->getParent()->getFileInfo()->fileName() );
    ui->resolutionValueLabel->setText( QString::number( m_clip->getParent()->getWidth() )
                                       + " x " + QString::number( m_clip->getParent()->getHeight() ) );
    ui->fpsValueLabel->setText( QString::number( m_clip->getParent()->getFps() ) );
    ui->snapshotLabel->setPixmap( m_clip->getParent()->getSnapshot().scaled( 128, 128, Qt::KeepAspectRatio ) );
    setWindowTitle( m_clip->getParent()->getFileInfo()->fileName() + " " + tr( "properties" ) );

    const QPushButton* button = ui->buttonBox->button( QDialogButtonBox::Apply );
    Q_ASSERT( button != NULL);
    connect( button, SIGNAL( clicked() ), this, SLOT( apply() ) );

    QStringListModel*   model = new QStringListModel( m_clip->getMetaTags(), this );
    ui->metaTagsView->setModel( model );

    connect( ui->addTagsButton, SIGNAL( clicked() ), this, SLOT( addTagsRequired() ) );
    connect( ui->deleteTagsButton, SIGNAL( clicked() ), this, SLOT( removeTagsRequired() ) );
}

MediaProperty::~MediaProperty()
{
    delete ui;
}

void MediaProperty::changeEvent( QEvent *e )
{
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void    MediaProperty::apply()
{
    QStringListModel* model = dynamic_cast<QStringListModel*>( ui->metaTagsView->model() );
    if ( model == NULL )
        return ;
    m_clip->setMetaTags( model->stringList() );
}

void    MediaProperty::addTagsRequired()
{
    bool                ok;
    QString             newTags = QInputDialog::getText( this, tr( "New tags edition" ),
                                            tr( "Enter tags (you can enter multiple tags, separated by a comma)" ),
                                            QLineEdit::Normal, "", &ok );
    if ( ok == true && newTags.length() > 0 )
    {
        QStringListModel*   model = dynamic_cast<QStringListModel*>( ui->metaTagsView->model() );
        if ( model == NULL )
            return ;
        QStringList         list = model->stringList();
        QStringList         toAdd = newTags.split( ",", QString::SkipEmptyParts );
        list.append( toAdd );
        model->setStringList( list );
    }
}

void    MediaProperty::removeTagsRequired()
{
    QStringListModel*   model = dynamic_cast<QStringListModel*>( ui->metaTagsView->model() );
    if ( model == NULL )
        return ;
    QItemSelectionModel*    selected = ui->metaTagsView->selectionModel();
    QModelIndexList         listSelected = selected->selectedIndexes();
    QStringList             list = model->stringList();
    while ( listSelected.empty() == false )
    {
        QVariant    elem = model->data( listSelected.first(), Qt::DisplayRole );
        list.removeOne( elem.toString() );
        listSelected.removeFirst();
    }
    model->setStringList( list );
}
