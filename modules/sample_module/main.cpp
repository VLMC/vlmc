#include "../../src/API/vlmc_module.h"

vlmc_module_begin()
vlmc_module_end()

static  vlmc_return_type_t  Open( void* pv_module )
{
    create_p_module( pv_module );

    vlmc_add_variable_int( p_module, "test_var", 0 );
    return VLMC_SUCCESS;
}

