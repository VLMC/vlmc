/*****************************************************************************
 * ProjectWizard.h: Project wizard
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>

class WelcomePage;

class ProjectWizard : public QWizard
{
    Q_OBJECT

    public:
         enum { Page_Welcome,
                Page_Open,
                Page_General, Page_Video };

         ProjectWizard( QWidget* parent = 0 );
         ~ProjectWizard();

    protected slots:
        virtual void    accept();
        virtual void    reject();

    private slots:
        void            showHelp();
};

#endif
