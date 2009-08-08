#include <QtDebug>
#include <QTime>
#include <QPushButton>
#include <QStringListModel>
#include <QInputDialog>

#include "MediaProperty.h"
#include "ui_MediaProperty.h"

MediaProperty::MediaProperty( Media* media, QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::MediaProperty ),
    m_media( media )
{
    QTime   duration;
    duration = duration.addMSecs( m_media->getLength() );

    ui->setupUi(this);
    connect( this, SIGNAL( accepted() ), this, SLOT( deleteLater() ) );
    connect( this, SIGNAL( rejected() ), this, SLOT( deleteLater() ) );
    ui->durationValueLabel->setText( duration.toString( "hh:mm:ss" ) );
    ui->nameValueLabel->setText( m_media->getFileInfo()->fileName() );
    ui->resolutionValueLabel->setText( QString::number( m_media->getWidth() )
                                       + " x " + QString::number( m_media->getHeight() ) );
    ui->fpsValueLabel->setText( QString::number( m_media->getFps() ) );
    ui->snapshotLabel->setPixmap( m_media->getSnapshot().scaled( 128, 128, Qt::KeepAspectRatio ) );
    setWindowTitle( m_media->getFileInfo()->fileName() + " " + tr( "properties" ) );

    const QPushButton* button = ui->buttonBox->button( QDialogButtonBox::Apply );
    Q_ASSERT( button != NULL);
    connect( button, SIGNAL( clicked() ), this, SLOT( apply() ) );

    QStringListModel*   model = new QStringListModel( media->getMetaTags(), this );
    ui->metaTagsView->setModel( model );

    connect( ui->addTagButton, SIGNAL( clicked() ), this, SLOT( addTagRequired() ) );
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
    m_media->setMetaTags( model->stringList() );
}

void    MediaProperty::addTagRequired()
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
        ui->metaTagsView->update( QModelIndex() );
    }
}
