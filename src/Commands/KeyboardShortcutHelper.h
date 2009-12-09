/*****************************************************************************
 * KeyboardShortcutHelper.h: An helper to catch keyboard shortcut modifications
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <beauze.h@gmail.com>
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

#ifndef KEYBOARDSHORTCUTHELPER_H
#define KEYBOARDSHORTCUTHELPER_H

#include <QShortcut>
#include <QString>

class   KeyboardShortcutHelper : public QShortcut
{
    Q_OBJECT

    public:
        KeyboardShortcutHelper( const QString& name, QWidget* parent = NULL, bool menu = false );
        virtual ~KeyboardShortcutHelper()
        {
        }

    private:
        QString     m_name;
        bool        m_menu;
    private slots:
        void        shortcutUpdated( const QVariant& value );
    signals:
        void        changed( const QString&, const QString& );
};

#endif // KEYBOARDSHORTCUTHELPER_H
