/*****************************************************************************
 * Transcode.h : File Transcoder
 *****************************************************************************
 * copyright (c) 2008-2009 the vlmc team
 *
 * authors: clement chavance <chavance.c@gmail.com>
 *
 * this program is free software; you can redistribute it and/or
 * modify it under the terms of the gnu general public license
 * as published by the free software foundation; either version 2
 * of the license, or (at your option) any later version.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * gnu general public license for more details.
 *
 * you should have received a copy of the gnu general public license
 * along with this program; if not, write to the free software
 * foundation, inc., 51 franklin street, fifth floor, boston ma 02110-1301, usa.
 *****************************************************************************/

#ifndef TRANSCODE_H
#define TRANSCODE_H

#include <QDialog>
#include <QString>
#include <QEvent>

#include "ui_transcode.h"

class Transcode : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( Transcode )
    public:
        static Transcode *instance( QWidget* parent = 0 );

    protected:
        virtual void changeEvent( QEvent *e );

    private:
        explicit Transcode( QWidget *parent = 0 );
        ~Transcode() {}
        Ui::Transcode m_ui;
        QString m_origVidPath;

        static Transcode *m_instance;

    private slots:
        void on_browseFileButton_clicked();

        void on_dialogButtonBox_accepted();
        void on_dialogButtonBox_rejected();

        void on_addProfile_clicked();
        void on_editProfile_clicked();
        void on_deleteProfile_clicked();
};

#endif
