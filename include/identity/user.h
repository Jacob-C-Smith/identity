/** !
 * User 
 * 
 * @file identity/user.h
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
#include <core/sha.h>

/// data
#include <data/array.h>
#include <data/dict.h>

/// reflection
#include <reflection/json.h>

// structure declarations
struct user_s;

// type definitions
typedef struct user_s user;

// forward declarations
/// constructors
int user_construct
(
    user **pp_user,

    size_t id,
    const char *p_name,
    const char *p_password,

    size_t org_id,

    size_t *p_groups, size_t groups_len,
    size_t *p_roles , size_t roles_len 
);

int user_from_json ( user **pp_user, json_value *p_value );

/// print
int user_print ( user *p_user );

void *user_key_accessor ( user *p_user );
int user_comparator ( size_t id_a, size_t id_b );
int user_password_comparator ( void *id_a, void *id_b );
void *user_password_key_accessor ( user *p_user );

int user_name_get ( user *p_user, char *_name );

/// pack 
int user_pack ( void *p_buffer, const user *const p_user );

/// unpack
int user_unpack ( void *const p_value, void *p_buffer );
