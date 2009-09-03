#include <QtDebug>
#include "vlmc_module.h"

vlmc_return_type_t  vlmc_add_variable_int( vlmc_module_t* p_module, const char* psz_varname, int value )
{
    Q_UNUSED( p_module );
    Q_UNUSED( psz_varname );
    Q_UNUSED( value );
    qDebug() << "Setting variable";

    return VLMC_SUCCESS;
}
