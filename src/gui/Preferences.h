#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGui/QWidget>
#include <QTranslator>
#include <QApplication>
#include <QSettings>
#include <QtDebug>

namespace Ui {
    class Preferences;
}

class Preferences : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Preferences )
public:
    explicit Preferences( QWidget *parent = 0 );
    virtual ~Preferences();

protected:
    virtual void changeEvent( QEvent *e );

private:
    Ui::Preferences *m_ui;
    static QTranslator* m_currentLang;

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonApply_clicked();
};

#endif // PREFERENCES_H
