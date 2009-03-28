/*****************************************************************************
 * Preferences.h: Preferences window
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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QtGui/QWidget>
#include <QTranslator>
#include <QApplication>
#include <QSettings>
#include <QtDebug>
#include "ui_Preferences.h"

class Preferences : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( Preferences )
public:
    explicit Preferences( QWidget *parent = 0 );
    virtual ~Preferences();

    static  void    changeLang(QString lang);

protected:
    virtual void changeEvent( QEvent *e );

private:
    Ui::Preferences m_ui;
    static QTranslator* m_currentLang;

private slots:
    void on_pushButtonCancel_clicked();
    void on_pushButtonApply_clicked();
};

#endif // PREFERENCES_H
