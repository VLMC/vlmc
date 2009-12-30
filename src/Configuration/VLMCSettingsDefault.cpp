#include <QVariant>

#include "VLMCSettingsDefault.h"
#include "SettingsManager.h"

void    VLMCSettingsDefault::load( const QString& part )
{
    SettingsManager::getInstance()->addNewSettingsPart( part );
    VLMCSettingsDefault::loadVLMCDefaults( part );
    VLMCSettingsDefault::loadlanguageDefaults( part );

    return ;
}

void    VLMCSettingsDefault::loadVLMCDefaults( const QString& part )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();
    QVariant defaultOutputPFS = "25";
    QVariant defaultPreviewFPS = "25";
    QVariant defaultTrackNb = "64";

    settingsMan->setValue( part,
                           "VLMCOutPutFPS", defaultOutputPFS  );
    settingsMan->setValue( part,
                           "VLMCPreviewFPS",
                           defaultPreviewFPS  );
    settingsMan->setValue( part,
                           "VLMCTracksNb",
                           defaultTrackNb  );

    return ;
}

void    VLMCSettingsDefault::loadlanguageDefaults( const QString& part )
{
}
