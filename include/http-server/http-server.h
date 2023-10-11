#pragma once

// Standard library
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// sync submodule
#include <sync/sync.h>

// socket submodule
#include <socket/socket.h>

// dict submodule
#include <dict/dict.h>

// array submodule
#include <array/array.h>

// json submodule
#include <json/json.h>

// queue submodule
#include <queue/queue.h>

// stack submodule
#include <stack/stack.h>

// tuple submodule
#include <tuple/tuple.h>

// http submodule
#include <http/http.h>

// Set the reallocator for the dict submodule
#ifdef DICT_REALLOC
    #undef DICT_REALLOC
    #define DICT_REALLOC(p, sz) realloc(p, sz)
#endif

// Set the reallocator for the array submodule
#ifdef ARRAY_REALLOC
    #undef ARRAY_REALLOC
    #define ARRAY_REALLOC(p, sz) realloc(p, sz)
#endif

// Set the reallocator for the JSON submodule
#ifdef JSON_REALLOC
    #undef JSON_REALLOC
    #define JSON_REALLOC(p, sz) realloc(p, sz)
#endif

// Set the reallocator for the queue submodule
#ifdef QUEUE_REALLOC
    #undef QUEUE_REALLOC
    #define QUEUE_REALLOC(p, sz) realloc(p, sz)
#endif

// Set the reallocator for the stack submodule
#ifdef STACK_REALLOC
    #undef STACK_REALLOC
    #define STACK_REALLOC(p, sz) realloc(p, sz)
#endif

// Set the reallocator for the tuple submodule
#ifdef TUPLE_REALLOC
    #undef TUPLE_REALLOC
    #define TUPLE_REALLOC(p, sz) realloc(p, sz)
#endif

// Memory management macro
#ifndef HTTP_SERVER_REALLOC
    #define HTTP_SERVER_REALLOC(p, sz) realloc(p,sz)
#endif

// Forward declarations
struct http_server_s;

// Type definitions
typedef struct http_server_s http_server;

/** !
 * Allocate memory for an HTTP server
 * 
 * @param pp_http_server return
 * 
 * @return 1 on success, 0 on error
*/
int http_server_create ( http_server **const pp_http_server );

/** !
 * Construct an HTTP server from a config file
 * 
 * @param pp_http_server return
 * @param p_path         path to config file
 * 
 * @return 1 on success, 0 on error 
 * 
*/
int http_server_construct ( http_server **const pp_http_server, const char *const p_path );

/** !
 * TCP accept callback
 * 
 * @param _socket the new socket
 * @param ip_address the ip address
 * @param port the port
 * @param p_http_server the http server receiving the request
 * 
 * @return 1 on success, 0 on error
*/
int http_server_accept ( socket_tcp _socket, unsigned long ip_address, unsigned short port, http_server *const p_http_server );

/** !
 * Destroy and deallocate an HTTP server and all it's contents.
 * 
 * @param pp_http_server return
 * 
 * @return 1 on success, 0 on error 
*/
int http_server_destroy ( http_server **const pp_http_server );
