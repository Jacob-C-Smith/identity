/** !
 * Organization
 *
 * @file identity/org.h
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
struct org_s;

// type definitions
typedef struct org_s org;

// forward declarations
/// constructors
int org_construct
(
    org **pp_org,

    size_t id,
    const char *p_name
);

int org_from_json ( org **pp_org, json_value *p_value );

/// print
int org_print ( org *p_org );

void *org_key_accessor ( org *p_org );
int org_comparator ( org *p_a, org *p_b );

/// pack 
int org_pack ( void *p_buffer, const org *const p_org );

/// unpack
int org_unpack ( void *const p_value, void *p_buffer );
