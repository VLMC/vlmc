#include "Preferences.h"
#include "ui_Preferences.h"

QTranslator* Preferences::m_currentLang = NULL;

Preferences::Preferences( QWidget *parent ) : QWidget( parent ), m_ui( new Ui::Preferences )
{
    m_ui->setupUi( this );
    m_ui->comboBoxLanguage->addItem( tr( "English" ), "" );
    m_ui->comboBoxLanguage->addItem( tr( "French" ), "fr" );
    m_ui->comboBoxLanguage->addItem( tr( "Spanish" ), "es" );
}

Preferences::~Preferences()
{
    delete m_ui;
}

void Preferences::changeEvent( QEvent *e )
{
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void Preferences::on_pushButtonCancel_clicked()
{
    this->close();
}

void Preferences::on_pushButtonApply_clicked()
{
    QSettings settings;
    QString lang = settings.value( "Lang" ).toString();
    QString langValue = this->m_ui->comboBoxLanguage->itemData( this->m_ui->comboBoxLanguage->currentIndex() ).toString();
    if (this->m_currentLang != NULL )
    {
        qApp->removeTranslator( this->m_currentLang );
        delete this->m_currentLang;
        this->m_currentLang = NULL;
    }
    if ( langValue != "" )
    {
        m_currentLang = new QTranslator();
        m_currentLang->load( "vlmc_" + langValue, QApplication::applicationDirPath() + "/../" );
        qApp->installTranslator( m_currentLang );
    }
    this->close();
}
