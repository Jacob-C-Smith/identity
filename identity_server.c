/** !
 * identity server
 * 
 * @file identity_server.c
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
#include <core/log.h>
#include <core/pack.h>

// identity
#include <identity/identity.h>
#include <identity/user.h>
#include <identity/role.h>
#include <identity/group.h>
#include <identity/org.h>

// forward declarations
/// pack/unpack
//// string
int string_pack   ( void *p_buffer, const void *const p_value );
int string_unpack ( void *const p_value, void *p_buffer );

// entry point
int main ( int argc, const char *argv[] )
{

    // initialized data
    identity *p_identity   = NULL;
    org      *_p_org    [] = { NULL, NULL };
    user *p_user = NULL;

    // construct an identity server
    identity_construct(&p_identity);

    // construct acme
    {

        // initialized data
        role  *_p_roles [] = { NULL, NULL, NULL, NULL, NULL, NULL };
        group *_p_groups[] = { NULL, NULL, NULL, NULL };
        user  *_p_users [] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

        // construct organization
        (void) org_construct(&_p_org[0], 0, "acme");
        (void) identity_org_add(p_identity, _p_org[0]);

        // add roles
        {

            // initialized data
            const char *_owner_permissions[] = { "*:*" };
            const char *_admin_permissions[] = { "read:*", "write:*", "manage:users", "manage:billing" };
            const char *_editor_permissions[] = { "read:docs/*", "write:docs/*", "read:issues/*", "write:issues/*" };
            const char *_viewer_permissions[] = { "read:*" };
            const char *_billing_permissions[] = { "read:billing", "write:billing" };
            const char *_support_permissions[] = { "read:tickets/*", "write:tickets/*" };

            // construct roles
            (void) role_construct(&_p_roles[0], 0, "owner"  , 0, _owner_permissions  , 1),
            (void) role_construct(&_p_roles[1], 1, "admin"  , 0, _admin_permissions  , 4),
            (void) role_construct(&_p_roles[2], 2, "editor" , 0, _editor_permissions , 4),
            (void) role_construct(&_p_roles[3], 3, "viewer" , 0, _viewer_permissions , 1),
            (void) role_construct(&_p_roles[4], 4, "billing", 0, _billing_permissions, 2),
            (void) role_construct(&_p_roles[5], 5, "support", 0, _support_permissions, 2);

            // add roles to organization
            for (size_t i = 0; i < sizeof(_p_roles) / sizeof(*_p_roles); i++)
                (void) identity_role_add(p_identity, _p_roles[i]);
        }

        // add groups
        {

            // initialized data
            const size_t _dev_roles[]     = { 2 };
            const size_t _finance_roles[] = { 4 };
            const size_t _ops_roles[]     = { 1 };
            const size_t _support_roles[] = { 5 };

            // construct groups
            (void) group_construct(&_p_groups[0], 0, "dev"    , 0, _dev_roles    , 1),
            (void) group_construct(&_p_groups[1], 1, "finance", 0, _finance_roles, 1),
            (void) group_construct(&_p_groups[2], 2, "ops"    , 0, _ops_roles    , 1),
            (void) group_construct(&_p_groups[3], 3, "support", 0, _support_roles, 1);

            // add groups to organization
            for (size_t i = 0; i < sizeof(_p_groups) / sizeof(*_p_groups); i++)
                (void) identity_group_add(p_identity, _p_groups[i]);
        }

        // add users
        {

            // initialized data
            const size_t _dev_group    [] = { 0 };
            const size_t _finance_group[] = { 1 };
            const size_t _ops_group    [] = { 2 };
            const size_t _support_group[] = { 3 };

            const size_t _zoe_roles  [] = { 0 };

            // construct users
            (void) user_construct(&_p_users[0], 0, "Alice", "a", 0, _dev_group, 1, NULL      , 0),
            (void) user_construct(&_p_users[1], 1, "Bob"  , "b", 0, _dev_group, 1, NULL      , 0),
            (void) user_construct(&_p_users[2], 2, "Carol", "c", 0, _dev_group, 1, NULL      , 0),
            (void) user_construct(&_p_users[3], 3, "Dana" , "d", 0, _dev_group, 1, NULL      , 0),
            (void) user_construct(&_p_users[4], 4, "Erin" , "e", 0, _dev_group, 1, NULL      , 0),
            (void) user_construct(&_p_users[5], 5, "Frank", "f", 0, _dev_group, 1, NULL      , 0),
            (void) user_construct(&_p_users[6], 6, "Zoe"  , "z", 0, _dev_group, 1, _zoe_roles, 1);

            // add users
            for (size_t i = 0; i < sizeof(_p_users)/sizeof(*_p_users); i++)
                (void) identity_user_add(p_identity, _p_users[i]);
        }

        // log
        log_info("[identity] Constructed identity server\n");
    }
    
    // construct g10
    /*
    {

        // initialized data
        role  *_p_roles [] = { NULL };
        group *_p_groups[] = { NULL };
        user  *_p_users [] = { NULL };

        // construct organization
        (void) org_construct(&_p_org[0], 0, "g10");
        (void) identity_org_add(p_identity, _p_org[0]);

        // add roles
        {

            // initialized data
            const char *_owner_permissions[] = { "*:*" };

            // construct roles
            (void) role_construct(&_p_roles[0], 0, "owner", 0, _owner_permissions, 1);

            // add roles to organization
            for (size_t i = 0; i < sizeof(_p_roles) / sizeof(*_p_roles); i++)
                (void) identity_role_add(p_identity, _p_roles[i]);
        }

        // add groups
        {

            // initialized data
            const size_t _dev_roles[] = { };

            // construct groups
            (void) group_construct(&_p_groups[0], 0, "dev", 0, _dev_roles, 1);

            // add groups to organization
            for (size_t i = 0; i < sizeof(_p_groups) / sizeof(*_p_groups); i++)
                (void) identity_group_add(p_identity, _p_groups[i]);
        }

        // add users
        {

            // initialized data
            const size_t _zoe_roles[] = { 0 };

            // construct users
            (void) user_construct(&_p_users[0], 0, "jacob", "abc123", 0, NULL, 0, _zoe_roles, 1);

            // add users
            for (size_t i = 0; i < sizeof(_p_users)/sizeof(*_p_users); i++)
                (void) identity_user_add(p_identity, _p_users[i]);
        }

        // log
        log_info("[identity] Constructed identity server\n");
    }
    */

    // print the identity server
    identity_print(p_identity);

    sleep(300);
    
    // success
    return EXIT_SUCCESS;
}

int string_pack ( void *p_buffer, const void *const p_value )
{

    // done
    return pack_pack(p_buffer, "%s", p_value);
}

int string_unpack ( void *const p_value, void *p_buffer )
{

    // done
    return pack_unpack(p_buffer, "%s", p_value);
}
