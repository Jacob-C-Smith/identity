/** !
 * Group
 *
 * @file identity/group.h
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
struct group_s;

// type definitions
typedef struct group_s group;

// forward declarations
/// constructors
int group_construct
(
    group **pp_group,

    size_t id,
    const char *p_name,
    size_t org_id,
    size_t _roles[],
    size_t _roles_length
);

int group_from_json ( group **pp_group, json_value *p_value );

/// print
int group_print ( group *p_group );

void *group_key_accessor ( group *p_group );
int group_comparator ( size_t id_a, size_t id_b );

/// pack 
int group_pack ( void *p_buffer, const group *const p_group );

/// unpack
int group_unpack ( void *const p_value, void *p_buffer );
