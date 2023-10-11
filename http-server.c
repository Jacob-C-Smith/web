#include <http-server/http-server.h>

struct http_server_s
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

int http_server_create ( http_server **const pp_http_server )
{

    // Argument check
    if ( pp_http_server == (void *) 0 ) goto no_http_server;

    // Initialized data
    http_server *p_http_server = HTTP_SERVER_REALLOC(0, sizeof(http_server));

    // Error check
    if ( p_http_server == (void *) 0 ) goto no_mem;

    // Zero set the struct
    memset(p_http_server, 0, sizeof(http_server));

    // Return a pointer to the caller
    *pp_http_server = p_http_server;

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_http_server:
                #ifndef NDEBUG
                    printf("[http server] Null pointer provided for parameter \"pp_http_server\" in call to function \"%s\"\n", __FUNCTION__);
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

int http_server_construct ( http_server **const pp_http_server, const char *const p_path )
{

    // Argument check
    if ( pp_http_server == (void *) 0 ) goto no_http_server;

    // Initialized data
    http_server    *p_http_server = 0;
    size_t          len           = load_file(p_path, 0, false);
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
    if ( http_server_create(&p_http_server) == 0 ) goto failed_to_allocate_http_server;

    // Populate the http server struct
    *p_http_server = (http_server)
    {
        .server =
        {
            .port   = port_number,
            .socket = _socket
        }
    };

    // Return a pointer to the caller
    *pp_http_server = p_http_server;

    // Success  
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_http_server:
                #ifndef NDEBUG
                    printf("[http server] Null pointer provided for parameter \"pp_http_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }

        // HTTP server errors
        {
            failed_to_allocate_http_server:
                #ifndef NDEBUG
                    printf("[http server] Call to function \"http_server_create\" returned an erroneous value in call to function \"%s\"\n", __FUNCTION__);
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

int http_server_start ( http_server *const p_http_server )
{

    // Set the running flag
    p_http_server->context.running = true;

    // Run
    while ( p_http_server->context.running )
    {
        
        // Get a new client
        socket_tcp_listen(p_http_server->server.socket, &http_server_accept, p_http_server);

        /*
        

        // Process new connections
        if ( queue_empty(p_chat_room_server->p_maybe_clients) == false )
        {
            
            // For each possible new connection
            while ( queue_empty(p_chat_room_server->p_maybe_clients) == false )
            {

                // Uninitialized data
                char _message[255];

                // Initialized data
                chat_room_client *p_client = 0;

                // Get a client
                queue_dequeue(p_chat_room_server->p_maybe_clients, &p_client);
                                
                // Try to get the sign in json object
                if ( socket_tcp_receive(p_client->_socket_tcp, _message, 255 ) )
                {
                    
                    // Initialized data
                    json_value *p_value = 0;

                    // Parse the JSON object
                    (void) parse_json_value(_message, 0, &p_value);

                    // Parse the properties
                    if ( p_value->type == JSON_VALUE_OBJECT )
                    {

                        // Initialized data
                        dict *p_dict = p_value->object;

                        strcpy(p_client->name, ((json_value *)dict_get(p_dict, "name"))->string);

                        p_client->signed_on = true;

                        free_json_value(p_value);

                        // Add the client to the dictionary
                        dict_add(p_chat_room_server->p_clients, p_client->name, p_client);

                        // Increment the user count
                        p_chat_room_server->user_count++;

                        // Log
                        printf("%s joined the chatroom\n", p_client->name);

                        while ( socket_tcp_receive(p_client->_socket_tcp, _message, 255 ));

                        socket_tcp_send(p_client->_socket_tcp, p_chat_room_server->welcome_message, strlen(p_chat_room_server->welcome_message));

                        continue;
                    }
                    
                }

                // Still waiting on the client
                queue_enqueue(p_chat_room_server->p_def_clients, p_client);
            }
            
            while ( queue_empty(p_chat_room_server->p_def_clients) == false )
            {
                
                // Initialized data
                chat_room_client *p_client = 0;

                queue_dequeue(p_chat_room_server->p_def_clients, &p_client);
                queue_enqueue(p_chat_room_server->p_maybe_clients, p_client);
            }
        }

        // Get each user
        dict_values(p_chat_room_server->p_clients, _clients);

        // Iterate over each user
        for (size_t i = 0; i < p_chat_room_server->user_count; i++)
        {

            // Uninitialized data
            char _message[255];
            char send_message[512];
            
            memset(_message, '\0', 255);

            // Initialized data
            chat_room_client *p_client = _clients[i];
            
            // Maybe get a message from the user
            if ( socket_tcp_receive(p_client->_socket_tcp, _message, 255 ) )
            {

                if ( strncmp(_message, "/quit", 5) == 0 )
                {

                    printf("%s left\n", p_client->name);
                    socket_tcp_destroy(&p_client->_socket_tcp);
                    continue;
                }

                size_t msg_len = 0;

                while (_message[msg_len] != '\0' && _message[msg_len] != '\n' && _message[msg_len] != '\r' )
                {
                    msg_len++;
                }
                
                for (size_t j = 0; j < p_chat_room_server->user_count; j++)
                {

                    // Prevent echo
                    if (strcmp(_clients[j]->name, p_client->name) == 0 ) 
                    {
                        // Send the message
                        int n = snprintf(send_message, 512, "\r%s > \0", _clients[j]->name);
                        socket_tcp_send(_clients[j]->_socket_tcp, send_message, n);
                        continue;
                    };

                    int n = snprintf(send_message, 512, "\r%s > %s\r%s > \0",p_client->name, _message, _clients[j]->name);

                    // Send the message
                    socket_tcp_send(_clients[j]->_socket_tcp, send_message, n);
                }

                while ( socket_tcp_receive(p_client->_socket_tcp, _message, 255 ));
            }
        }
        */
    }
    
    // Success
    return 1;
}

int http_server_accept ( socket_tcp _socket, unsigned long ip_address, unsigned short port, http_server *const p_http_server )
{

    char response[65535+1];

    char buf[65535+1];
    char cl[6+1];
    size_t l = load_file("index.html", buf, false);
    snprintf(cl, l, "%d", l);

    http_serialize_response(response, HTTP_OK, "%c %cl", "Keep-Alive", cl);
    
    strcat(response, "\n\r");
    strcat(response, buf);

    socket_tcp_send(_socket, response, strlen(response));

    // Success
    return 1;
}

int http_server_destroy ( http_server **const pp_http_server )
{

    // Argument check
    if ( pp_http_server == (void *) 0 ) goto no_http_server;

    // Initialized data
    //

    // Success
    return 1;

    // Error handling
    {

        // Argument errors
        {
            no_http_server:
                #ifndef NDEBUG
                    printf("[http server] Null pointer provided for parameter \"pp_http_server\" in call to function \"%s\"\n", __FUNCTION__);
                #endif

                // Error
                return 0;
        }
    }
}
