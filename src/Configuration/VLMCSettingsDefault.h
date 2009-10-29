#ifndef VLMCSETTINGSDEFAULT_H
#define VLMCSETTINGSDEFAULT_H

class VLMCSettingsDefault
{
    public:
        static void load( void );

    private:
        static void loadVLMCDefaults( void );
        static void loadlanguageDefaults( void );
    private:
        VLMCSettingsDefault();
        ~VLMCSettingsDefault();
        VLMCSettingsDefault( const VLMCSettingsDefault& );
        VLMCSettingsDefault&  operator = ( const VLMCSettingsDefault& );
};

#endif
