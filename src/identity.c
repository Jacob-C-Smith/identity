/** !
 * Identity 
 * 
 * @file src/identity.c
 * 
 * @author Jacob Smith
 */

// header
#include <identity/identity.h>

// structure definitions
struct identity_s
{
    bool running;

    binary_tree *p_users;
    binary_tree *p_orgs;
    binary_tree *p_roles;
    binary_tree *p_groups;
    binary_tree *p_reverse_users;

    thread_pool     *p_thread_pool;
    socket_tcp       _socket;
    parallel_thread *p_listener_thread;
};

int identity_server_accept( socket_tcp _socket_tcp, socket_ip_address ip_address, socket_port port_number, identity *p_identity )
{

    // initialized data
    size_t      len      = 0;
    char       *p_buffer = 0;
    json_value *p_value  = 0;
    user       *p_maybe_user = NULL;

    // log the connection
    log_info("[identity] Accepted incoming connection from %hhu.%hhu.%hhu.%hhu:%hu\n", 
            (ip_address >> 24) & 0xFF, 
            (ip_address >> 16) & 0xFF, 
            (ip_address >>  8) & 0xFF, 
            (ip_address >>  0) & 0xFF, 
            
            port_number
    );

    // parse the request
    {

        // get the length of the request
        socket_tcp_receive(_socket_tcp, &len, sizeof(size_t));

        // error check
        if ( 4096 < len ) goto too_long;

        // allocate a buffer for the rest of the message
        p_buffer = default_allocator(0, len);
        if ( NULL == p_buffer ) goto no_mem;

        // receive the rest of the message
        socket_tcp_receive(_socket_tcp, p_buffer, len);

        // parse the request
        json_value_parse(p_buffer, 0, &p_value);

        // print the parsed JSON value
        json_value_print(p_value);
        fflush(stdout);
    }

    // process the request
    {
        
        // initialized data
        dict *p_dict = p_value->object;
        json_value *p_user = dict_get(p_dict, "user"),
                   *p_pass = dict_get(p_dict, "pass");
        char       *p_pass16 = p_pass->string;
        sha256_hash _pass = { 0 };

        // convert hex string p_pass16 into binary sha256_hash _pass
        {
            unsigned char *dst = (unsigned char *)&_pass;

            for (size_t i = 0; i < sizeof(sha256_hash); i++)
            {
            char c1 = p_pass16[2 * i];
            char c2 = p_pass16[2 * i + 1];

            int hi = (c1 >= '0' && c1 <= '9') ? (c1 - '0') :
                 (c1 >= 'a' && c1 <= 'f') ? (c1 - 'a' + 10) :
                 (c1 >= 'A' && c1 <= 'F') ? (c1 - 'A' + 10) : -1;

            int lo = (c2 >= '0' && c2 <= '9') ? (c2 - '0') :
                 (c2 >= 'a' && c2 <= 'f') ? (c2 - 'a' + 10) :
                 (c2 >= 'A' && c2 <= 'F') ? (c2 - 'A' + 10) : -1;

            if (hi < 0 || lo < 0) { log_error("[identity] Non-hex character in SHA-256 hex"); goto parse_error; }

            dst[i] = (unsigned char)((hi << 4) | lo);
            }
        }
        
        // reverse lookup
        binary_tree_search(p_identity->p_reverse_users, &_pass, (void **)&p_maybe_user);

        // get username
        char _name[64+1] = { 0 };
        user_name_get(p_maybe_user, _name);

        p_maybe_user = ( strcmp(_name, p_user->string) ) ? log_error("[identity] Error!"), NULL : p_maybe_user;

        if (p_maybe_user) user_print(p_maybe_user);
        else log_error("[identity] User not found");

    }

    // serialize the response
    {

        // initialized data
        char _buf[1024] = { 0 };
        json_value _val = 
        { 
            .type = JSON_VALUE_STRING,
            .string = (p_maybe_user) ? "okay" : "not okay"
        };
        size_t len = json_value_serialize(&_val, &_buf[8]);

        // set the length
        *(size_t *)_buf = len;

        // send the result
        socket_tcp_send(_socket_tcp, _buf, 8 + len);
    }

    // success
    return 1;

    // error handling
    {

        // socket errors
        {
            too_long:
            no_mem:
            parse_error:
                return 0;
        }
    }
}

int identity_listener ( identity *p_identity )
{

    // log a message
    log_info("[identity] Listening for incoming connections...\n");

    // listen for incoming connections
    while ( p_identity->running )
        socket_tcp_listen(p_identity->_socket, (fn_socket_tcp_accept *)identity_server_accept, p_identity);

    return 0;
}

int identity_construct
(
    identity **pp_identity

    // const char *p_users_path,
    // const char *p_orgs_path,
    // const char *p_roles_path,
    // const char *p_groups_path
)
{

    // argument check
    if ( NULL == pp_identity ) goto no_identity;

    // initialized data
    identity *p_identity = default_allocator(0, sizeof(identity));

    // error check
    if ( NULL == p_identity ) goto no_mem;

    // construct sets
    {

        // construct organizations
        binary_tree_construct(
            &p_identity->p_orgs,
            (fn_comparator *)   org_comparator, // comparator
            (fn_key_accessor *) org_key_accessor, // key accessor
            2048
        );

        // construct roles
        binary_tree_construct(
            &p_identity->p_roles,
            (fn_comparator *)   role_comparator, // comparator
            (fn_key_accessor *) role_key_accessor, // key accessor
            2048
        );

        // construct groups
        binary_tree_construct(
            &p_identity->p_groups,
            (fn_comparator *)   group_comparator, // comparator
            (fn_key_accessor *) group_key_accessor, // key accessor
            2048
        );

        // construct users
        binary_tree_construct(
            &p_identity->p_users,
            (fn_comparator *)   user_comparator, // comparator
            (fn_key_accessor *) user_key_accessor, // key accessor
            2048
        );

        // reverse lookup 
        binary_tree_construct(
            &p_identity->p_reverse_users,
            (fn_comparator *)   user_password_comparator, // comparator
            (fn_key_accessor *) user_password_key_accessor, // key accessor
            2048
        );
    }

    // construct networking stuff
    {
        
        // construct a thread pool
        thread_pool_construct(&p_identity->p_thread_pool, 4);

        // construct a socket
        socket_tcp_create(&p_identity->_socket, socket_address_family_ipv4, 6708);

        // set the running flag
        p_identity->running = true;

        // construct a listener thread
        parallel_thread_start(&p_identity->p_listener_thread, (fn_parallel_task *)identity_listener, p_identity);
    }

    // return a pointer to the caller 
    *pp_identity = p_identity;
    
    // success
    return 1;

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"pp_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
           }
    
        // standard library errors
        {
            no_mem:
                #ifndef NDEBUG
                    log_error("[identity] Failed to allocate memory in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

// if ( NULL ==  p_users_path ) goto no_users_path;
// if ( NULL ==   p_orgs_path ) goto no_orgs_path;
// if ( NULL ==  p_roles_path ) goto no_roles_path;
// if ( NULL == p_groups_path ) goto no_groups_path;

// no_users_path:
//     #ifndef NDEBUG
//         log_error("[identity] Null pointer provided for parameter \"p_users_path\" in call to function \"%s\"", __FUNCTION__);
//     #endif

//     // error
//     return 0;

// no_orgs_path:
//     #ifndef NDEBUG
//         log_error("[identity] Null pointer provided for parameter \"p_orgs_path\" in call to function \"%s\"", __FUNCTION__);
//     #endif

//     // error
//     return 0;

// no_roles_path:
//     #ifndef NDEBUG
//         log_error("[identity] Null pointer provided for parameter \"p_roles_path\" in call to function \"%s\"", __FUNCTION__);
//     #endif

//     // error
//     return 0;

// no_groups_path:
//     #ifndef NDEBUG
//         log_error("[identity] Null pointer provided for parameter \"p_groups_path\" in call to function \"%s\"", __FUNCTION__);
//     #endif

//     // error
//     return 0;

int identity_user_lookup ( identity *p_identity, size_t id, user **pp_user )
{

    // argument check
    if ( NULL == p_identity ) goto no_identity;
    if ( NULL == pp_user ) goto no_user;

    // lookup user
    return binary_tree_search(p_identity->p_users, (void *)id, (void **)pp_user);

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;

            no_user:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"pp_user\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int identity_user_reverse_lookup ( identity *p_identity, size_t id, user **pp_user )
{

    // argument check
    if ( NULL == p_identity ) goto no_identity;
    if ( NULL == pp_user ) goto no_user;

    // lookup user
    return binary_tree_search(p_identity->p_reverse_users, (void *)id, (void **)pp_user);

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;

            no_user:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"pp_user\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int identity_org_add ( identity *p_identity, org *p_org )
{
    
    // argument check
    if ( NULL == p_identity ) goto no_identity;
    if ( NULL ==      p_org ) goto no_org;

    // done
    return binary_tree_insert(p_identity->p_orgs, p_org);

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;

            no_org:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_org\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int identity_role_add ( identity *p_identity, role *p_role )
{
    
    // argument check
    if ( NULL == p_identity ) goto no_identity;
    if ( NULL ==     p_role ) goto no_role;

    // done
    return binary_tree_insert(p_identity->p_roles, p_role);

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;

            no_role:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_role\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int identity_group_add ( identity *p_identity, group *p_group )
{
    
    // argument check
    if ( NULL == p_identity ) goto no_identity;
    if ( NULL ==    p_group ) goto no_group;

    // done
    return binary_tree_insert(p_identity->p_groups, p_group);

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;

            no_group:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_group\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int identity_user_add ( identity *p_identity, user *p_user )
{
    
    // argument check
    if ( NULL == p_identity ) goto no_identity;
    if ( NULL ==     p_user ) goto no_user;

    // done
    return binary_tree_insert(p_identity->p_users        , p_user) && 
           binary_tree_insert(p_identity->p_reverse_users, p_user);

    // error handling
    {

        // argument errors
        {
            no_identity:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_identity\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;

            no_user:
                #ifndef NDEBUG
                    log_error("[identity] Null pointer provided for parameter \"p_user\" in call to function \"%s\"", __FUNCTION__);
                #endif

                // error
                return 0;
        }
    }
}

int identity_print ( identity *p_identity )
{

    // argument check
    // if ( NULL == p_identity ) goto no_identity;

    // initialized data
    //

    binary_tree_traverse_inorder(p_identity->p_orgs, (fn_binary_tree_traverse *) org_print);

    binary_tree_traverse_inorder(p_identity->p_roles, (fn_binary_tree_traverse *) role_print);

    binary_tree_traverse_inorder(p_identity->p_groups, (fn_binary_tree_traverse *) group_print);

    binary_tree_traverse_inorder(p_identity->p_users, (fn_binary_tree_traverse *) user_print);

    // success
    return 1;
}