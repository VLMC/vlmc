#ifndef VLMCSETTINGSDEFAULT_H
#define VLMCSETTINGSDEFAULT_H

class VLMCSettingsDefault
{
    public:
        static void load( const QString& part );

    private:
        static void loadVLMCDefaults( const QString& part );
        static void loadlanguageDefaults( const QString& part );
    private:
        VLMCSettingsDefault();
        ~VLMCSettingsDefault();
        VLMCSettingsDefault( const VLMCSettingsDefault& );
        VLMCSettingsDefault&  operator = ( const VLMCSettingsDefault& );
};

#endif
