#ifndef PANEL_H
#define PANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QString>
#include <QToolButton>

class   Panel : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Panel )
    public:
        Panel( QWidget* parent = 0 );
        ~Panel();

        void    addButton( const QString& label,
                           const QString& iconPath,
                           int number);
        void    show();

    private:
        QVBoxLayout*        m_layout;
        QButtonGroup*       m_buttons;
        QToolButton*        m_firstButton;
        int                 m_firstButtonNb;
        static const int    M_ICON_HEIGHT;

    private slots:
        void    switchPanel( int panel );

    signals:
        void   changePanel( int panel );
};


#endif /* !PANEL_H */
