#include <web/server.h>

struct web_server_s
{
    struct
    {
        unsigned short port;
        socket_tcp     socket;
    } server;

    struct
    {
        bool running;
    } context;
};

/**!
 * Return the size of a file IF buffer == 0 ELSE read a file into buffer
 * 
 * @param path path to the file
 * @param buffer buffer
 * @param binary_mode "wb" IF true ELSE "w"
 * 
 * @return 1 on success, 0 on error
 */
size_t load_file ( const char *path, void *buffer, bool binary_mode )
{

    // Argument checking 
    if ( path == 0 ) goto no_path;

    // Initialized data
    size_t  ret = 0;
    FILE   *f   = fopen(path, (binary_mode) ? "rb" : "r");
    
    // Check if file is valid
    if ( f == NULL ) goto invalid_file;

    // Find file size and prep for read
    fseek(f, 0, SEEK_END);
    ret = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    // Read to data
    if ( buffer ) 
        ret = fread(buffer, 1, ret, f);

    // The file is no longer needed
    fclose(f);
    
    // Success
    return ret;

    // Error handling
    {

        // Argument errors
        {
            no_path:
                #ifndef NDEBUG
                    printf("[http server] Null path provided to function \"%s\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // File errors
        {
            invalid_file:
                #ifndef NDEBUG
                    printf("[Standard library] Failed to load file \"%s\". %s\n",path, strerror(errno));
                #endif

                // Error
                return 0;
        }
    }
}

int web_server_create ( web_server **const pp_web_server )
{

    // Argument check
    if ( pp_web_server == (void *) 0 ) goto no_web_server;

    // Initialized data
    web_server *p_web_server = HTTP_SERVER_REALLOC(0, sizeof(web_server));

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
                    printf("[http server] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
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
    web_server    *p_web_server = 0;
    size_t          len           = load_file(p_path, 0, true);
    char           *p_buffer      = calloc(len+1, sizeof(char));
    json_value     *p_value       = 0,
                   *p_port_number = 0;
    unsigned short  port_number   = 0;
    socket_tcp      _socket       = 0;

    // Load the file
    if ( load_file(p_path, p_buffer, true) == 0 ) goto failed_to_load_file;

    // Parse the file text into a json value
    if ( parse_json_value(p_buffer, 0, &p_value) == 0 ) goto failed_to_parse_json_value;

    // Parse the json object
    if ( p_value->type == JSON_VALUE_OBJECT )
    {

        // Initialized data
        dict *p_dict = p_value->object;

        // Get the port number
        p_port_number = dict_get(p_dict, "port");

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
    }

    // Construct http server struct fields
    if ( socket_tcp_create(&_socket, socket_address_family_ipv4, port_number) == 0 ) goto failed_to_create_tcp_socket;
 
    // Allocate an htktp server
    if ( web_server_create(&p_web_server) == 0 ) goto failed_to_allocate_web_server;

    // Populate the http server struct
    *p_web_server = (web_server)
    {
        .server =
        {
            .port   = port_number,
            .socket = _socket
        }
    };

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
                    printf("[http server] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // HTTP server errors
        {
            failed_to_allocate_web_server:
                #ifndef NDEBUG
                    printf("[http server] Call to function \"web_server_create\" returned an erroneous value in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error 
                return 0;
        }

        // JSON errors
        {
            failed_to_parse_json_value:
                #ifndef NDEBUG
                    printf("[http server] Failed to parse JSON text in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error 
                return 0;
        }

        // Socket errors
        {
            failed_to_create_tcp_socket:
                #ifndef NDEUBG
                    printf("[http server] Call to function \"socket_tcp_create\" returned an erroneous value in call to function \"%s\"\n", __FUNCTION__);
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

    wait:
    if ( socket_tcp_receive(_socket,request,65535) == 1 )
    {
        printf("\n\n%s\n\n\n", request);
        memset(request, 0, 65535);
        goto done;
    }
    goto wait;
    done:;

    char cl[6+1];
    size_t l = load_file("index.html", buf, true);
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
                    printf("[http server] Null pointer provided for parameter \"pp_web_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}
