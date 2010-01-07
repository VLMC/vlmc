/*****************************************************************************
 * About.h: About dialog
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Christophe Courtaut <christophe.courtaut@gmail.com>
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

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include "ui_About.h"

class QPlainTextEdit;

class About : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( About )
public:
    static About* instance();

protected:
    virtual void    changeEvent( QEvent *e );

private:
    explicit        About( QWidget *parent = 0 );
    void            setText( const QString& filename, QPlainTextEdit* widget );
    Ui::AboutVLMC   m_ui;
    static About*   m_instance;
};

#endif // ABOUT_H
