/** !
 * Role
 *
 * @file identity/role.h
 *
 * @author Jacob Smith
 */

// standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// gsdk
#include <gsdk.h>

/// core
#include <core/log.h>
#include <core/pack.h>

/// data
#include <data/array.h>
#include <data/dict.h>

/// reflection
#include <reflection/json.h>

// structure declarations
struct role_s;

// type definitions
typedef struct role_s role;

// forward declarations
/// constructors
int role_construct
(
    role **pp_role,

    size_t id,
    const char *p_name,
    size_t org_id,
    char *_p_permissions[],
    size_t permissions_length
);

int role_from_json ( role **pp_role, json_value *p_value );

/// print
int role_print ( role *p_role );

void *role_key_accessor ( role *p_role );
int role_comparator ( size_t id_a, size_t id_b );

/// pack 
int role_pack ( void *p_buffer, const role *const p_role );

/// unpack
int role_unpack ( void *const p_value, void *p_buffer );
