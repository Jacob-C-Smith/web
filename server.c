#include <web/server.h>

int web_server_create ( web_server **const pp_web_server )
{

    // Argument check
    if ( pp_web_server == (void *) 0 ) goto no_web_server;

    // Initialized data
    web_server *p_web_server = WEB_REALLOC(0, sizeof(web_server));

    // Error check
    if ( p_web_server == (void *) 0 ) goto no_mem;

    // Zero set the struct
    memset(p_web_server, 0, sizeof(web_server));

    // Return a pointer to the caller
    *pp_web_server = p_web_server;

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_web_server:
                #ifndef NDEBUG
                    printf("[web] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // Standard library errors
        {
            no_mem:
                #ifndef NDEBUG
                    printf("[Standard Library] Failed to allocate memory in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}

int web_server_construct ( web_server **const pp_web_server, const char *const p_path )
{

    // Argument check
    if ( pp_web_server == (void *) 0 ) goto no_web_server;

    // Initialized data
    web_server *p_web_server = 0;
    size_t      len          = web_load_file(p_path, 0, true);
    char       *p_buffer     = calloc(len+1, sizeof(char));
    json_value *p_value      = 0;

    // Load the file
    if ( web_load_file(p_path, p_buffer, true) == 0 ) goto failed_to_load_file;

    // Parse the file text into a json value
    if ( parse_json_value(p_buffer, 0, &p_value) == 0 ) goto failed_to_parse_json_value;

    // Parse the json value into a web server
    if ( web_server_from_json_value(&p_web_server, p_value) == 0 ) goto failed_to_load_web_server;

    // Return a pointer to the caller
    *pp_web_server = p_web_server;

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_web_server:
                #ifndef NDEBUG
                    printf("[web] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // JSON errors
        {
            failed_to_parse_json_value:
                #ifndef NDEBUG
                    printf("[web] Failed to parse JSON text in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error 
                return 0;
        }

        // Standard library errors
        {
            failed_to_load_file:
                #ifndef NDEBUG
                    printf("[Standard Library] Failed to open file \"%s\" in call to function \"%s\"\n", p_path, __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // Web errors
        {
            failed_to_load_web_server:
                #ifndef NDEBUG
                    printf("[Standard Library] Failed to load web server in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}

int web_server_from_json_value ( web_server **const pp_web_server, const json_value *const p_value )
{

    // Argument check
    if ( pp_web_server == (void *) 0 ) goto no_web_server;
    if ( p_value       == (void *) 0 ) goto no_value;

    // Initialized data
    web_server     *p_web_server        = 0;
    json_value     *p_port_number       = 0,
                   *p_routes            = 0;
    unsigned short  port_number         = 0;
    socket_tcp      _socket             = 0;
    dict           *p_web_server_routes = 0; 

    // Initialized data
    // Parse the json object
    if ( p_value->type == JSON_VALUE_OBJECT )
    {

        // Initialized data
        dict *p_dict = p_value->object;

        // Get the port number
        p_port_number = dict_get(p_dict, "port");

        // Get the routes
        p_routes = dict_get(p_dict, "routes");

        // Error checking
        // TODO: 
    }

    // Default
    // TODO:

    // Parse the JSON properties
    {

        // Store the port number
        if ( p_port_number->type == JSON_VALUE_INTEGER )
            port_number = p_port_number->integer;
        // else
            // TODO: 
        
        // Parse the routes
        if ( p_routes->type == JSON_VALUE_OBJECT )
        {

            // Initialized data
            dict *p_dict = p_routes->object;
            char *p_routes[1024] = { 0 };
            size_t route_count = dict_keys(p_dict, 0);
            dict_keys(p_dict, &p_routes);
     
            // Construct a dictionary to store routes
            if ( dict_construct(&p_web_server_routes, route_count, 0) == 0 ) goto failed_to_construct_dict;

            // Iterate over each route
            for (size_t i = 0; i < route_count; i++)
            {
                
                // Initialized data
                json_value *p_route     = dict_get(p_dict, p_routes[i]);
                web_route  *p_web_route = 0;

                // Construct the route from the JSON value
                if ( web_route_from_json(&p_web_route, p_route) == 0 ) goto failed_to_construct_web_route;

                // Store the name of the web route
                strcpy(p_web_route->name, p_routes[i]);

                // Store the route in the routes dictionary
                dict_add(p_web_server_routes, p_web_route->name, p_web_route->path);
            }

            // Error checking
            // TODO:         
        }
    }

    // Construct http server struct fields
    if ( socket_tcp_create(&_socket, socket_address_family_ipv4, port_number) == 0 ) goto failed_to_create_tcp_socket;
 
    // Allocate an http server
    if ( web_server_create(&p_web_server) == 0 ) goto failed_to_allocate_web_server;

    // Populate the http server struct
    *p_web_server = (web_server)
    {
        .server =
        {
            .port   = port_number,
            .socket = _socket
        },
        .routes = p_web_server_routes
    };

    // Return a pointer to the caller
    *pp_web_server = p_web_server;

    // Success  
    return 1;

    // TODO:
    failed_to_construct_web_route:
    failed_to_construct_dict:
        // Error
        return 0;

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_web_server:
                #ifndef NDEBUG
                    printf("[web] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;

            no_value:
                #ifndef NDEBUG
                    printf("[web] Null pointer provided for parameter \"p_value\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;            
        }

        // HTTP server errors
        {
            failed_to_allocate_web_server:
                #ifndef NDEBUG
                    printf("[web] Call to function \"web_server_create\" returned an erroneous value in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error 
                return 0;
        }

        // Socket errors
        {
            failed_to_create_tcp_socket:
                #ifndef NDEUBG
                    printf("[web] Call to function \"socket_tcp_create\" returned an erroneous value in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}

int web_server_start ( web_server *const p_web_server )
{

    // Set the running flag
    p_web_server->context.running = true;

    // Run
    while ( p_web_server->context.running )
    {
        
        // Get a new client
        socket_tcp_listen(p_web_server->server.socket, &web_server_accept, p_web_server);
    }
    
    // Success
    return 1;
}

int web_server_accept ( socket_tcp _socket, unsigned long ip_address, unsigned short port, web_server *const p_web_server )
{

    char response[65535+1];

    char buf[65535+1];

    char request[65535+1];
    char *path = 0;
    enum http_request_type_e request_type;
    char *file_path = 0;

    wait:
    if ( socket_tcp_receive(_socket,request,65535) == 1 )
    {
        //printf("\n\n%s\n\n\n", request);
        http_parse_request(request, &request_type, &path, 0);
        printf("%s %s\n", http_request_types[request_type-1], path);
        file_path = dict_get(p_web_server->routes, path);
        memset(request, 0, 65535);
        goto done;
    }
    goto wait;
    done:;

    char cl[6+1];
    size_t l = web_load_file(file_path, buf, true);
    snprintf(cl, 6, "%d", l);

    http_serialize_response(response, HTTP_OK, "%c %cl", "Keep-Alive", cl);
    
    strcat(response, "\n\r");
    strcat(response, buf);

    socket_tcp_send(_socket, response, strlen(response));

    // Success
    return 1;
}

int web_server_destroy ( web_server **const pp_web_server )
{

    // Argument check
    if ( pp_web_server == (void *) 0 ) goto no_web_server;

    // Initialized data
    //

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_web_server:
                #ifndef NDEBUG
                    printf("[web] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}
