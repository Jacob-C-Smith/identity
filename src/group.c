/** !
 * groupanization
 *
 * @file src/group.c
 * 
 * @author Jacob Smith
 */

// header
#include <identity/group.h>

// structure definitions
struct group_s
{
    size_t id;
    char   _name[64+1];
};

int group_construct
(
    group **pp_group,

    size_t id,
    const char *p_name,
    size_t org_id,
    size_t _roles[],
    size_t _roles_length
)
{

    // argument check
    if ( NULL == pp_group ) goto no_group;
    if ( NULL == p_name ) goto no_name;

    // initialized data
    group *p_group = default_allocator(NULL, sizeof(group));

    // error check
    if ( NULL == p_group ) goto no_mem;

    // populate the group struct
    *p_group = (group)
    {
        .id    = id,
        ._name = { 0 }
    };

    // copy the name
    strncpy(p_group->_name, p_name, sizeof(p_group->_name) - 1);
    p_group->_name[strlen(p_name)] = '\0';

    // return a pointer to the caller
    *pp_group = p_group;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_group:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"pp_group\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"p_name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_email:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"p_email\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_groups:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"p_groups\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_roles:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"p_roles\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    
        // standard library errors
        {
            no_mem_1: p_group = default_allocator(p_group, 0);
            no_mem:
                #ifndef NDEBUG
                    log_error("[standard library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int group_from_json
(
    group        **pp_group,
    json_value  *p_value
)
{

    // argument check
    if ( NULL ==  pp_group ) goto no_group;
    if ( NULL == p_value ) goto no_value;

    // initialized data
    group        *p_group    = default_allocator(NULL, sizeof(group));
    dict       *p_dict   = NULL;
    json_value *p_name   = NULL,
               *p_group_id = NULL;

    // error check
    if ( NULL == p_group ) goto no_mem;

    // type check
    if ( JSON_VALUE_OBJECT != p_value->type ) goto wrong_type;

    // store the object
    p_dict = p_value->object;

    // get the name
    p_name = dict_get(p_dict, "name");
    p_group_id = dict_get(p_dict, "group_id");

    // error check
    if ( NULL ==   p_name ) goto no_name;
    if ( NULL == p_group_id ) goto no_group_id;
    
    // type check
    if (  JSON_VALUE_STRING !=   p_name->type ) goto name_wrong_type;
    if ( JSON_VALUE_INTEGER != p_group_id->type ) goto group_id_wrong_type;

    // populate the group struct
    *p_group = (group)
    {
        .id    = p_group_id->integer,
        ._name = { 0 }
    };

    // copy the name
    strncpy(p_group->_name, p_name->string, sizeof(p_group->_name) - 1);
    p_group->_name[strlen(p_name->string)] = '\0';

    // return a pointer to the caller
    *pp_group = p_group;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_group:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"pp_group\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_value:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"p_value\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }

        // json errors
        {
            wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [group] Parameter \"p_value\" must be of type [ object ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
            
            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [group] Parameter \"p_value\" missing property \"name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_group_id:
                #ifndef NDEBUG
                    log_error("[identity] [group] Parameter \"p_value\" missing property \"group_id\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            name_wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [group] Property \"name\" of parameter \"p_value\" must be of type [ string ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
            
            group_id_wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [group] Property \"group_id\" of parameter \"p_value\" must be of type [ integer ] in call to function \"%s\"\n", __FUNCTION__);
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

int group_print ( group *p_group )
{

    // argument check
    if ( NULL == p_group ) goto no_group;

    // formatting
    log_info("group @%p\n", (void *)p_group);
    printf(" - ID   : %lld\n", p_group->id);
    printf(" - Name : %s\n", p_group->_name);

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_group:
                #ifndef NDEBUG
                    log_error("[identity] [group] Null pointer provided for parameter \"p_group\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int group_comparator ( size_t id_a, size_t id_b )
{

    // success
    return id_b - id_a;
}

void *group_key_accessor ( group *p_group )
{

    // success
    return (void *)p_group->id;
}

int group_pack ( void *p_buffer, const group *const p_group )
{

    // initialized data
    char *p_offset = (char *) p_buffer;

    // pack group id
    p_offset += pack_pack(p_offset, "%i64", p_group->id),

    // pack the name
    p_offset += pack_pack(p_offset, "%s", p_group->_name);

    // done
    return (void *)p_offset - (void *)p_buffer;
}

int group_unpack ( void *const p_value, void *p_buffer )
{

    // 

    // done
    return 0;
}
