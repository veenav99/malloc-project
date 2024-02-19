#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define TASK_COUNT 50 
#define ALLOC_COUNT 120 

void task1() {
    for ( int i = 0; i < TASK_COUNT; i++ ) {
        for ( int j = 0; j < ALLOC_COUNT; j++ ) {
            void *ptr = malloc( 1 );
            free( ptr );
        }
    }
}

void task2() {
    void *ptrArray[ ALLOC_COUNT ];
    memset( ptrArray , 0 , sizeof( void *)* ALLOC_COUNT );

    for ( int i = 0; i < TASK_COUNT; i++ ) {
        for ( int j = 0; j < ALLOC_COUNT; j++ ) {
            if( ptrArray[ j ] == NULL ){
                ptrArray[ j ] = malloc( 1 );
            }
        }

        for ( int j = 0; j < ALLOC_COUNT; j++ ) {
            if( ptrArray[ j ] != NULL ){
                free(ptrArray[ j ] );
                ptrArray[ j ] = NULL;
            }
        }
    }
}

void task3() {
    void *ptrArray[ ALLOC_COUNT ];
    memset( ptrArray , 0 , sizeof( void *)* ALLOC_COUNT );

    for ( int i = 0; i < TASK_COUNT; i++ ) {
        for ( int j = 0; j < ALLOC_COUNT; j++ ) {
            if ( rand() % 2 == 0 && ptrArray[ j ] == NULL ) {
                ptrArray[ j ] = malloc( 1 );
            } else {
                if ( j > 0 && ptrArray[ j - 1 ] != NULL ) {
                    free( ptrArray[ j - 1 ] );
                    ptrArray[ j - 1 ] = NULL;
                }
            }
        }
        for ( int j = 0; j < ALLOC_COUNT; j++ ) {
            if ( ptrArray[ j ] != NULL ) {
                free( ptrArray[ j ] );
                ptrArray[ j ] = NULL;
            }
        }
    }
}

// Additional tasks can be added as needed

int main() {
    struct timeval start , end;
    long elapsedTime;

    gettimeofday( &start , NULL );
    srand( 0 );

    // Perform tasks
    task1();
    task2();
    task3();

    // Measure elapsed time
    gettimeofday( &end , NULL );
    elapsedTime = ( end.tv_sec - start.tv_sec ) * 1000000 + ( end.tv_usec - start.tv_usec );

    printf( "Total time elapsed: %ld microseconds\n" , elapsedTime );

    return 0;
}