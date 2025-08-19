/** !
 * Identity 
 * 
 * @file identity/identity.h
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
#include <core/socket.h>
#include <core/pack.h>

/// data
#include <data/array.h>
#include <data/binary.h>

/// performance
#include <performance/thread_pool.h>

// auth
#include <identity/org.h>
#include <identity/role.h>
#include <identity/group.h>
#include <identity/user.h>

// structure declarations
struct identity_s;

// type definitions
typedef struct identity_s identity;

// forward declarations
/// constructors
int identity_construct ( identity **pp_identity );

/// accessors
int identity_user_lookup ( identity *p_identity, size_t id, user **pp_user );


/// mutators
int identity_org_add ( identity *p_identity, org *p_org );
int identity_role_add ( identity *p_identity, role *p_role );
int identity_group_add ( identity *p_identity, group *p_group );
int identity_user_add ( identity *p_identity, user *p_user );

int identity_print ( identity *p_identity );
