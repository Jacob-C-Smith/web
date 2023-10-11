/** !
 * An example program for the http server
 * 
 * @author Jacob Smith
 * 
 * @file main.c
*/

// Standard library
#include <stdio.h>

// http server 
#include <http-server/http-server.h>

// Entry point
int main ( int argc, const char *argv[] )
{

    // Initialized data
    http_server *p_http_server = 0;

    // Construct an http server
    if ( http_server_construct(&p_http_server, "config.json") == 0 ) goto failed_to_construct_http_server;

    // Start the http server
    if ( http_server_start(p_http_server) == 0 ); 

    // Success
    return EXIT_SUCCESS;

    // Error handling
    {

        failed_to_construct_http_server:

            // Write an error message to standard out
            printf("Failed to start HTTP server!\n");

            // Error
            return EXIT_FAILURE;
    }
}
