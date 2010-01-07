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
#include <QProgressDialog>
#include <QTimer>
#include "vlc/vlc.h"

//#include "ui_transcode.h"

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
        bool m_doTranscode( const QString &transStr );
        void m_initProgressDialog();
        void m_releaseVLCRessources();

        //Ui::Transcode m_ui;
        QString m_origVidPath;

        libvlc_exception_t m_vlcEx;
        libvlc_instance_t *m_libvlc;
        libvlc_media_t *m_vlcMedia;
        libvlc_media_player_t *m_vlcMp;
        libvlc_event_manager_t *m_vlcEM;

        qint64 m_origMediaLength;

        QProgressDialog *m_progress;
        QTimer          *m_timer;

        bool m_running;

        static Transcode *m_instance;
        static void m_callback(const libvlc_event_t *event, void *ptr);

    private slots:
        void on_browseFileButton_clicked();

        void on_dialogButtonBox_accepted();
        void on_dialogButtonBox_rejected();

        void on_addProfile_clicked();
        void on_editProfile_clicked();
        void on_deleteProfile_clicked();

        void calcTranscodePercentage();
        void cancelTranscode();
};

#endif
