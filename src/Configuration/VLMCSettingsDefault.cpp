#include <QVariant>

#include "VLMCSettingsDefault.h"
#include "SettingsManager.h"

void    VLMCSettingsDefault::load( void )
{
    SettingsManager::getInstance()->addNewSettingsPart( "default" );
    VLMCSettingsDefault::loadVLMCDefaults();
    VLMCSettingsDefault::loadlanguageDefaults();

    return ;
}

void    VLMCSettingsDefault::loadVLMCDefaults( void )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();
    QVariant defaultOutputPFS = "25";
    QVariant defaultPreviewFPS = "25";
    QVariant defaultTrackNb = "64";

//    settingsMan->setValue( "VLMCOutPutFPS", defaultOutputPFS  );
//    settingsMan->setValue( "VLMCPreviewFPS", defaultPreviewFPS  );
//    settingsMan->setValue( "VLMCTracksNb", defaultTrackNb  );
//
//    return ;
}

void    VLMCSettingsDefault::loadlanguageDefaults( void )
{
}
