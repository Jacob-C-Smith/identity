/** !
 * identity client
 * 
 * @file main.c
 * 
 * @author Jacob Smith
 */

// standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// gsdk
#include <gsdk.h>
#include <core/log.h>
#include <core/pack.h>


// forward declarations
/** !
 * Print a usage message to standard out
 * 
 * @param argv0 the name of the program
 * 
 * @return void
 */
void print_usage ( const char *argv0 );

/** !
 * Parse command line arguments
 * 
 * @param argc            the argc parameter of the entry point
 * @param argv            the argv parameter of the entry point
 * 
 * @return void on success, program abort on failure
 */
void parse_command_line_arguments ( int argc, const char *argv[] );

// entry point
int main ( int argc, const char *argv[] )
{

    // todo

    // success
    return EXIT_SUCCESS;
}

void print_usage ( const char *argv0 )
{

    // argument check
    if ( argv0 == (void *) 0 ) exit(EXIT_FAILURE);

    // Print a usage message to standard out
    printf("Usage: %s ...\n", argv0);

    // done
    return;
}

void parse_command_line_arguments ( int argc, const char *argv[] )
{

    // If no command line arguments are supplied, run all the examples
    if ( argc == 1 ) goto default_options;

    // error check
    //

    // Iterate through each command line argument
    for (size_t i = 1; i < (size_t) argc; i++)
    {

        if ( strcmp(argv[i], "-n") == 0 )

            // Set the name
            ;//

        // Default
        else goto invalid_arguments;
    }
    
    // success
    return;

    // default
    default_options:
    {

        // success
        return;
    }

    // error handling
    {

        // argument errors
        {
            invalid_arguments:
                
                // Print a usage message to standard out
                print_usage(argv[0]);

                // Abort
                exit(EXIT_FAILURE);
        }
    }
}
