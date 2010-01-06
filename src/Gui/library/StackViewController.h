/*****************************************************************************
 * StackViewController.h
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Thomas Boquet <thomas.boquet@gmail.com>
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

#ifndef STACKVIEWCONTROLLER_H
#define STACKVIEWCONTROLLER_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QStack>

#include "StackViewNavController.h"
#include "ViewController.h"

class ImportController;

class StackViewController : public QWidget
{
    Q_OBJECT

public:
    StackViewController( QWidget* parent = 0, bool enableImport = true );
    ~StackViewController();

    void                    pushViewController( ViewController* viewController,
                                                bool animated = false );
    void                    popViewController( bool animated = false );
    const ViewController*   getCurrentViewController() const;

private:
    StackViewNavController*     m_nav;
    QPushButton*                m_importButton;
    QVBoxLayout*                m_layout;
    ViewController*             m_current;
    QStack<ViewController*>*    m_controllerStack;

public slots:
    void                        previous();

signals:
    void                        importRequired();
    void                        previousButtonPushed();

};

#endif // STACKVIEWCONTROLLER_H
