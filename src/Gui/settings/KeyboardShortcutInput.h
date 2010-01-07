/*****************************************************************************
 * KeyboardShortcutInput.h: Input widget for keybard shortcut
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef KEYBOARDSHORTCUTINPUT_H
#define KEYBOARDSHORTCUTINPUT_H

#include <QPushButton>

class QTimer;

class   KeyboardShortcutInput : public QPushButton
{
    Q_OBJECT

    public:
        KeyboardShortcutInput( const QString& name, const QString& initialValue = QString(), QWidget* parent = NULL );
    protected:
        virtual void    keyPressEvent( QKeyEvent* e );
        virtual void    mousePressEvent( QMouseEvent* e );
        virtual void    mouseReleaseEvent( QMouseEvent* e );
        void            release();

    protected slots:
        void            timeout();

    private:
        QString         m_name;
        bool            m_capturing;
        QTimer*         m_timer;
        unsigned int    m_current;
        int             m_shortcuts[4];
    signals:
        void            changed( const QString&, const QString& );
};

#endif // KEYBOARDSHORTCUTINPUT_H
