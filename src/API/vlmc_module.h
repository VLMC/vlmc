/*****************************************************************************
 * vlcm_module.h: Header to be used by effects modules
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#ifndef VLMC_MODULE_H
#define VLMC_MODULE_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    /* Output buffer */
    unsigned char*      p_buffer;
    /* Output width */
    unsigned int        i_width;
    /* Output height */
    unsigned int        i_height;
}   vlmc_output_t;

enum        vlmc_return_type_t
{
    VLMC_SUCCESS = 0,
    VLMC_INVALID_INPUT_PARAMETER,
    VLMC_OUT_OF_MEMORY,
    VLMC_UNEXPECTED_ERROR,
};

typedef     vlmc_return_type_t  (*open_callback_t)( void* );
typedef     vlmc_return_type_t  (*close_callback_t)( void* );
typedef     vlmc_return_type_t  (*process_callback_t)( unsigned char*, unsigned int );

typedef struct
{
    open_callback_t     pf_open;
    close_callback_t    pf_close;
    process_callback_t  pf_process;
}   vlmc_callback_t;

typedef struct
{
    char*               psz_name;
    float               f_ratio;
    vlmc_output_t*      p_output;
    vlmc_callback_t*    p_callbacks;
}   vlmc_module_t;

#define CPP_CONCATENACK( a, b )  y##__##z
#define CONCATENATE( a, b ) CPP_CONCATENACK( a, b )

#if defined(VLMC_MODULE) && defined( WIN32 )
#define VLMC_EXPORT     __declspec(dllexport)
#else
    #define VLMC_EXPORT
#endif

#define     vlmc_module_begin()                                                             \
    VLMC_EXPORT vlmc_return_type_t vlmc_module_entrypoint( vlmc_module_t* p_module )        \
    {

#define     vlmc_module_end()           \
    return VLMC_SUCCESS;                \
    }

#define     vlmc_set_callbacks( open, close )   \
    p_module->p_callbacks->pf_open = open;      \
    p_module->p_callbacks->pf_close = close;

#ifdef __cplusplus
}
#endif

#define create_p_module( void_var )    \
        vlmc_module_t* p_module = (vlmc_module_t*) void_var;\


/*
 * Function part :
 */

vlmc_return_type_t  vlmc_add_variable_int( vlmc_module_t* p_module, const char* psz_varname, int value );

#endif // VLMC_MODULE_H
