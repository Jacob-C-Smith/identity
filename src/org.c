/** !
 * Organization
 *
 * @file src/org.c
 * 
 * @author Jacob Smith
 */

// header
#include <identity/org.h>

// structure definitions
struct org_s
{
    size_t id;
    char   _name[64+1];
};

int org_construct
(
    org **pp_org,

    size_t id,
    const char *p_name
)
{

    // argument check
    if ( NULL == pp_org ) goto no_org;
    if ( NULL == p_name ) goto no_name;

    // initialized data
    org *p_org = default_allocator(NULL, sizeof(org));

    // error check
    if ( NULL == p_org ) goto no_mem;

    // populate the org struct
    *p_org = (org)
    {
        .id    = id,
        ._name = { 0 }
    };

    // copy the name
    strncpy(p_org->_name, p_name, sizeof(p_org->_name) - 1);
    p_org->_name[strlen(p_name)] = '\0';

    // return a pointer to the caller
    *pp_org = p_org;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_org:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"pp_org\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"p_name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_email:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"p_email\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_groups:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"p_groups\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_roles:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"p_roles\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    
        // standard library errors
        {
            no_mem_1: p_org = default_allocator(p_org, 0);
            no_mem:
                #ifndef NDEBUG
                    log_error("[standard library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int org_from_json
(
    org        **pp_org,
    json_value  *p_value
)
{

    // argument check
    if ( NULL ==  pp_org ) goto no_org;
    if ( NULL == p_value ) goto no_value;

    // initialized data
    org        *p_org    = default_allocator(NULL, sizeof(org));
    dict       *p_dict   = NULL;
    json_value *p_name   = NULL,
               *p_org_id = NULL;

    // error check
    if ( NULL == p_org ) goto no_mem;

    // type check
    if ( JSON_VALUE_OBJECT != p_value->type ) goto wrong_type;

    // store the object
    p_dict = p_value->object;

    // get the name
    p_name = dict_get(p_dict, "name");
    p_org_id = dict_get(p_dict, "org_id");

    // error check
    if ( NULL ==   p_name ) goto no_name;
    if ( NULL == p_org_id ) goto no_org_id;
    
    // type check
    if (  JSON_VALUE_STRING !=   p_name->type ) goto name_wrong_type;
    if ( JSON_VALUE_INTEGER != p_org_id->type ) goto org_id_wrong_type;

    // populate the org struct
    *p_org = (org)
    {
        .id    = p_org_id->integer,
        ._name = { 0 }
    };

    // copy the name
    strncpy(p_org->_name, p_name->string, sizeof(p_org->_name) - 1);
    p_org->_name[strlen(p_name->string)] = '\0';

    // return a pointer to the caller
    *pp_org = p_org;

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_org:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"pp_org\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_value:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"p_value\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }

        // json errors
        {
            wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [org] Parameter \"p_value\" must be of type [ object ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
            
            no_name:
                #ifndef NDEBUG
                    log_error("[identity] [org] Parameter \"p_value\" missing property \"name\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            no_org_id:
                #ifndef NDEBUG
                    log_error("[identity] [org] Parameter \"p_value\" missing property \"org_id\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;

            name_wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [org] Property \"name\" of parameter \"p_value\" must be of type [ string ] in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
            
            org_id_wrong_type:
                #ifndef NDEBUG
                    log_error("[identity] [org] Property \"org_id\" of parameter \"p_value\" must be of type [ integer ] in call to function \"%s\"\n", __FUNCTION__);
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

int org_print ( org *p_org )
{

    // argument check
    if ( NULL == p_org ) goto no_org;

    // formatting
    log_info("Org @%p\n", (void *)p_org);
    printf(" - ID   : %lld\n", p_org->id);
    printf(" - Name : %s\n", p_org->_name);

    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_org:
                #ifndef NDEBUG
                    log_error("[identity] [org] Null pointer provided for parameter \"p_org\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int org_comparator ( org *p_a, org *p_b )
{

    // success
    return p_b->id - p_a->id;
}

void *org_key_accessor ( org *p_org )
{

    // success
    return (void *)p_org->id;
}

int org_pack ( void *p_buffer, const org *const p_org )
{

    // initialized data
    char *p_offset = (char *) p_buffer;

    // pack org id
    p_offset += pack_pack(p_offset, "%i64", p_org->id),

    // pack the name
    p_offset += pack_pack(p_offset, "%s", p_org->_name);

    // done
    return (void *)p_offset - (void *)p_buffer;
}

int org_unpack ( void *const p_value, void *p_buffer )
{

    // 

    // done
    return 0;
}
