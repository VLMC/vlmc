#ifndef KEYBOARDSHORTCUTINPUT_H
#define KEYBOARDSHORTCUTINPUT_H

#include <QPushButton>
#include <QTimer>

class   KeyboardShortcutInput : public QPushButton
{
    Q_OBJECT

    public:
        KeyboardShortcutInput( QWidget* parent );
    protected:
        virtual void    keyPressEvent( QKeyEvent* e );
        virtual void    mousePressEvent( QMouseEvent* e );
        virtual void    mouseReleaseEvent( QMouseEvent* e );
        void            release();

    protected slots:
        void            timeout();

    private:
        bool            m_capturing;
        QTimer*         m_timer;
        unsigned int    m_current;
        int             m_shortcuts[4];
};

#endif // KEYBOARDSHORTCUTINPUT_H
