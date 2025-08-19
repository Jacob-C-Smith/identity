/** !
 * Role
 *
 * @file src/role.c
 * 
 * @author Jacob Smith
 */

// header
#include <identity/role.h>

// structure definitions
struct role_s
{
    size_t id;
    char   _name[64+1];
    size_t org_id;
    array *p_permissions;
};

int role_construct
(
    role **pp_role,

    size_t id,
    const char *p_name,
    size_t org_id,
    char *_p_permissions[],
    size_t permissions_length
)
{

    // argument check
    if ( NULL == pp_role ) goto no_role;
    if ( NULL ==  p_name ) goto no_name;

    // initialized data
    role *p_role = default_allocator(NULL, sizeof(role));

    // error check
    if ( NULL == p_role ) goto no_mem;

    // populate the role struct
    *p_role = (role)
    {
        .id    = id,
        ._name = { 0 }
    };

    // copy the name
    strncpy(p_role->_name, p_name, sizeof(p_role->_name) - 1);
    p_role->_name[strlen(p_name)] = '\0';

    // return a pointer to the caller
    *pp_role = p_role;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_role:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"pp_role\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"p_name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_email:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"p_email\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_groups:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"p_groups\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_roles:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"p_roles\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    
        // standard library errors
        {
            no_mem_1: p_role = default_allocator(p_role, 0);
            no_mem:
                #ifndef NDEBUG
                    log_error("[standard library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int role_from_json
(
    role        **pp_role,
    json_value  *p_value
)
{

    // argument check
    if ( NULL ==  pp_role ) goto no_role;
    if ( NULL == p_value ) goto no_value;

    // initialized data
    role        *p_role    = default_allocator(NULL, sizeof(role));
    dict       *p_dict   = NULL;
    json_value *p_name   = NULL,
               *p_role_id = NULL;

    // error check
    if ( NULL == p_role ) goto no_mem;

    // type check
    if ( JSON_VALUE_OBJECT != p_value->type ) goto wrong_type;

    // store the object
    p_dict = p_value->object;

    // get the name
    p_name = dict_get(p_dict, "name");
    p_role_id = dict_get(p_dict, "role_id");

    // error check
    if ( NULL ==   p_name ) goto no_name;
    if ( NULL == p_role_id ) goto no_role_id;
    
    // type check
    if (  JSON_VALUE_STRING !=   p_name->type ) goto name_wrong_type;
    if ( JSON_VALUE_INTEGER != p_role_id->type ) goto role_id_wrong_type;

    // populate the role struct
    *p_role = (role)
    {
        .id    = p_role_id->integer,
        ._name = { 0 }
    };

    // copy the name
    strncpy(p_role->_name, p_name->string, sizeof(p_role->_name) - 1);
    p_role->_name[strlen(p_name->string)] = '\0';

    // return a pointer to the caller
    *pp_role = p_role;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_role:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"pp_role\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_value:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"p_value\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }

        // json errors
        {
            wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [role] Parameter \"p_value\" must be of type [ object ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
            
            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [role] Parameter \"p_value\" missing property \"name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_role_id:
                #ifndef NDEBUG
                    log_error("[identity] [role] Parameter \"p_value\" missing property \"role_id\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            name_wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [role] Property \"name\" of parameter \"p_value\" must be of type [ string ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
            
            role_id_wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [role] Property \"role_id\" of parameter \"p_value\" must be of type [ integer ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }

        // standard library errors
        {
            no_mem:
                #ifndef NDEBUG
                    log_error("[standard library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int role_print ( role *p_role )
{

    // argument check
    if ( NULL == p_role ) goto no_role;

    // formatting
    log_info("role @%p\n", (void *)p_role);
    printf(" - ID   : %lld\n", p_role->id);
    printf(" - Name : %s\n", p_role->_name);

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_role:
                #ifndef NDEBUG
                    log_error("[identity] [role] Null pointer provided for parameter \"p_role\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int role_comparator ( size_t id_a, size_t id_b )
{

    // success
    return id_b - id_a;
}

void *role_key_accessor ( role *p_role )
{

    // success
    return (void *)p_role->id;
}

int role_pack ( void *p_buffer, const role *const p_role )
{

    // initialized data
    char *p_offset = (char *) p_buffer;

    // pack role id
    p_offset += pack_pack(p_offset, "%i64", p_role->id),

    // pack the name
    p_offset += pack_pack(p_offset, "%s", p_role->_name);

    // done
    return (void *)p_offset - (void *)p_buffer;
}

int role_unpack ( void *const p_value, void *p_buffer )
{

    // 

    // done
    return 0;
}
