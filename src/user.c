/** !
 * User 
 * 
 * @file src/user.c
 * 
 * @author Jacob Smith
 */

// header
#include <identity/user.h>

// structure definitions
struct user_s
{
    size_t       id;
    size_t       org_id;
    array       *p_groups;
    array       *p_roles;
    char        *p_name;
    sha256_hash  _password_hash;
};

// function definition
int number_pack ( void *p_buffer, const void *const p_value )
{

    // done
    return pack_pack(p_buffer, "%i64", p_value);
}

int user_construct
(
    user **pp_user,

    size_t id,
    const char *p_name,
    const char *p_password,

    size_t org_id,

    size_t *p_groups, size_t groups_len,
    size_t *p_roles , size_t roles_len 
)
{

    // argument check
    if ( NULL ==  pp_user ) goto no_user;
    if ( NULL ==   p_name ) goto no_name;
    if ( NULL == p_groups && 0 < groups_len ) goto no_groups;
    if ( NULL ==  p_roles && 0 <  roles_len ) goto no_roles;

    // initialized data
    user *p_user = default_allocator(NULL, sizeof(user));
    sha256_state _state = { 0 };

    // error check
    if ( NULL == p_user ) goto no_mem;

    // populate the user struct
    *p_user = (user)
    {
        .id       = id,
        .org_id   = org_id,
        .p_groups = NULL,
        .p_roles  = NULL,
        .p_name   = strdup(p_name),
    };

    // error check
    if ( NULL ==  p_user->p_name ) goto no_mem_1;

    // construct a group array
    if ( 0 == array_construct(&p_user->p_groups, groups_len + 1) ) goto no_mem_2;

    // construct a role array
    if ( 0 == array_construct(&p_user->p_roles, roles_len + 1) ) goto no_mem_4;

    // populate the group array
    for (size_t i = 0; i < groups_len; ++i)
        (void) array_add(p_user->p_groups, (void *) p_groups[i]);

    // populate the role array
    for (size_t i = 0; i < roles_len; ++i)
        (void) array_add(p_user->p_roles, (void *) p_roles[i]);
    
    // hash the password
    sha256_construct(&_state),
    sha256_update(&_state, (const unsigned char *) p_password, strlen(p_password)),
    sha256_final(&_state, p_user->_password_hash);

    // return a pointer to the caller
    *pp_user = p_user;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_user:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"pp_user\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_email:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_email\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_groups:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_groups\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_roles:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_roles\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    
        // standard library errors
        {
            no_mem_4: array_destroy(&p_user->p_groups, 0);
            no_mem_2:   p_user->p_name = default_allocator(p_user->p_name , 0);
            no_mem_1:           p_user = default_allocator(p_user         , 0);
            no_mem:
                #ifndef NDEBUG
                    log_error("[standard library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int user_from_json
(
    user       **pp_user,
    json_value  *p_value
)
{

    // argument check
    if ( NULL == pp_user ) goto no_user;
    if ( NULL == p_value ) goto no_value;

    // initialized data
    user *p_user = default_allocator(NULL, sizeof(user));
    dict *p_dict = NULL;
    

    // error check
    if ( NULL == p_user ) goto no_mem;

    // type check
    if ( JSON_VALUE_OBJECT != p_value->type ) goto wrong_type;

    // store the object
    p_dict = p_value->object;



    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_user:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"pp_user\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_value:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_value\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }

        // json errors
        {
            wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [user] Parameter \"p_value\" must be of type [ object ] in call to function \"%s\"\n", __FUNCTION__);
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

int user_print ( user *p_user )
{

    // argument check
    if ( NULL == p_user ) goto no_user;

    // formatting
    log_info("user @%p\n", (void *)p_user);
    printf(" - ID: %lld\n", p_user->id);
    printf(" - Organization ID: %lld\n", p_user->org_id);
    printf(" - Name: %s\n", p_user->p_name);
    printf(" - Pass: ");
    for (size_t i = 0; i < sizeof(sha256_hash); i++)
        printf("%hhx", p_user->_password_hash[i]);
    putchar('\n');
    printf(" - Roles[%d]: \n", array_size(p_user->p_roles));
    printf(" - Groups[%d]: \n", array_size(p_user->p_groups));

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_user:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_user\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int user_comparator ( size_t id_a, size_t id_b )
{

    // success
    return id_b - id_a;
}

int user_password_comparator ( void *id_a, void *id_b )
{

    // success
    return memcmp(id_a, id_b, sizeof(sha256_hash));
}

void *user_key_accessor ( user *p_user )
{

    // success
    return (void *)p_user->id;
}

void *user_password_key_accessor ( user *p_user )
{

    // success
    return (void *)&p_user->_password_hash;
}

int user_name_get ( user *p_user, char *_name )
{

    // argument check
    if ( NULL == p_user ) goto no_user;
    if ( NULL ==  _name ) goto no_name;

    // initialized data
    strcpy(_name, p_user->p_name);

    // success
    return 1;

    // error handling
    {

        // argument errors
        {

            no_user:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"p_user\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [user] Null pointer provided for parameter \"_name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int user_pack ( void *p_buffer, const user *const p_user )
{

    // initialized data
    char *p_offset = (char *) p_buffer;

    // pack user id
    p_offset += pack_pack(p_offset, "%i64", p_user->id),

    // pack organization id
    p_offset += pack_pack(p_offset, "%i64", p_user->org_id),

    // pack the groups
    p_offset += array_pack(p_offset, p_user->p_groups, number_pack),

    // pack the roles
    p_offset += array_pack(p_offset, p_user->p_roles, number_pack),

    // pack the name
    p_offset += pack_pack(p_offset, "%s", p_user->p_name);

    // done
    return (void *)p_offset - (void *)p_buffer;
}

int user_unpack ( void *const p_value, void *p_buffer )
{

    // 

    // done
    return 0;
}
