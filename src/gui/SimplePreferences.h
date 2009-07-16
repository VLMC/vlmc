#ifndef SIMPLEPREFERENCES_H
#define SIMPLEPREFERENCES_H

#include <QWidget>
#include <QString>
#include <QHash>
#include <QVBoxLayout>
#include <QButtonGroup>

class   PreferencesPanel : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( PreferencesPanel )

    public:
        PreferencesPanel( QWidget* parent = 0 );
        ~PreferencesPanel();
        void    addPanel( const QString& icon,
                          const QString& label,
                          const QString& name );
        static const int   ICON_HEIGHT;

    private:
        int                 m_nbPanel;
        QVBoxLayout*        m_layout;
        QButtonGroup*       m_buttonGroup;
        QHash<int, QString> m_panels;

    private slots:
        void    switchPanel( int panel );

    signals:
        void    panelSwitched( int panel );

};

class   SimplePreferences : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( SimplePreferences )

    public:
        SimplePreferences*  getInstance();
        void                releaseInstance();
        QWidget*            getWidget( const QString& name ) const;
        void                addWidget( const QString& name,
                                       QWidget* widget,
                                       const QString& icon,
                                       const QString& label );

    private:
        SimplePreferences();
        ~SimplePreferences();
        SimplePreferences( SimplePreferences const& sp );
        SimplePreferences&  operator =( SimplePreferences const& sp );

    private:
        static SimplePreferences*       m_instance;
        QHash<QString, QWidget*>        m_widgets;
        QWidget*                        m_currentWidget;
        PreferencesPanel*               m_panel;

    public slots:
        void    switchWidget( const QString& name );
};

#endif /* !SIMPLEPREFERENCES_H */
