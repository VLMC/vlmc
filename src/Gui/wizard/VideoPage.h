/*****************************************************************************
 * VideoPage.h: Wizard page for configuring video settings
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#ifndef VIDEOPAGE_H
#define VIDEOPAGE_H

#include <QWizardPage>
#include "ui_VideoPage.h"

class VideoPage : public QWizardPage
{
    Q_OBJECT
public:
    enum SampleRate
    {
        HZ_48000,
        HZ_44000,
        HZ_22000,
        HZ_11000
    };

    enum VideoPresets
    {
        PRESET_VideoCustom,
        PRESET_480i,    // SDTV/NTSC
        PRESET_576i,    // SDTV/PAL
        PRESET_480p,    // EDTV
        PRESET_576p,    // EDTV
        PRESET_720p,    // HDTV
        PRESET_1080i,   // HDTV
        PRESET_1080p,   // HDTV
    };

    enum AudioPresets
    {
        PRESET_AudioCustom,
        PRESET_STEREO,
        PRESET_MONO
    };

    VideoPage( QWidget *parent = 0 );

protected:
    void changeEvent( QEvent *e );
    virtual int nextId() const;
    virtual void initializePage();
    virtual bool validatePage();
    virtual void cleanupPage();

private slots:
    void updateVideoPresets();
    void updateAudioPresets();

private:
    void setVideoFormEnabled( bool enabled );
    void setAudioFormEnabled( bool enabled );
    void setVideoResolution( int width, int height );
    Ui::VideoPage ui;
};

#endif // VIDEOPAGE_H
