#include "Preferences.h"
#include "ui_Preferences.h"

Preferences::Preferences(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::Preferences)
{
    m_ui->setupUi(this);
    m_ui->comboBoxLanguage->addItem(tr( "English" ), "");
    m_ui->comboBoxLanguage->addItem(tr( "French" ), "fr");
    m_ui->comboBoxLanguage->addItem(tr( "Spanish" ), "es");
}

Preferences::~Preferences()
{
    delete m_ui;
}

void Preferences::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
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
    QTranslator translator;
    qDebug() << QApplication::applicationDirPath() + "/../" << "vlmc_" + (this->m_ui->comboBoxLanguage->itemData(this->m_ui->comboBoxLanguage->currentIndex())).toString();
    qDebug() << translator.load("vlmc_" + (this->m_ui->comboBoxLanguage->itemData(this->m_ui->comboBoxLanguage->currentIndex())).toString(), QApplication::applicationDirPath() + "/../");
    QApplication::installTranslator(&translator);
    this->close();
}
