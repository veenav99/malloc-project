#include "mymalloc.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#define MEMLENGTH 800000 //512
static double memory[ MEMLENGTH / 8 ];

// Helper function to round up to the nearest multiple of 8
static size_t roundUpTo8( size_t size ) {
    return ( size + 7 ) & ~7;
}

// Chunk structure representing both metadata and payload
typedef struct Chunk {
    size_t size;      // Total size of the chunk (excluding header)
    int32_t is_allocated;  // Flag indicating whether the chunk is allocated (1) or free (0)
    char padding[ 4 ]; // makes sure the data ends up at the end of the chunk struct
    char data[ 0 ]; // 0 length array to show end of struct
} Chunk;

void initializeMemory() {
    // Initialize the first chunk representing the entire memory
    Chunk *initialChunk = ( Chunk * )memory; //giving pointer to memory
    initialChunk -> size = MEMLENGTH * sizeof( double ) - sizeof( Chunk );
    initialChunk -> is_allocated = 0;  // Mark the initial chunk as free
}

Chunk* getNextChunk( Chunk * prev ){
    Chunk *next = ( Chunk * )( prev -> data + prev -> size );
    if( (char * )next + sizeof( Chunk ) >= (char *)memory + MEMLENGTH || next -> size == 0 ){
        return NULL;
    } else {
        return next;
    }
}

int splitChunk( Chunk *currentChunk , size_t size ) {
    // Calculate the size of the new free chunk
    if ( currentChunk -> size >= sizeof( Chunk ) + size ) {
        size_t remainingSize = currentChunk -> size - size - sizeof( Chunk );
        // Create a new free chunk
        Chunk *newChunk = ( Chunk * )( currentChunk -> data + size );
        newChunk -> size = remainingSize;
        newChunk -> is_allocated = 0;

        // Update metadata of the current chunk
        currentChunk -> size = size;
        return 1;
    }
    return 0;
}

void *mymalloc( size_t size , char *file , int line ) {
    size_t roundedSize = roundUpTo8( size ); 

    // If this is the first call to malloc, initialize the memory
    static int memoryInitialized = 0;
    if ( !memoryInitialized ) {
        initializeMemory();
        memoryInitialized = 1; //allocted memory
    }

    // Iterate through the memory array to find a free chunk
    Chunk *currentChunk = ( Chunk * )memory;
    while ( currentChunk != NULL ) {
        if ( !currentChunk -> is_allocated && currentChunk -> size >= roundedSize ) {
            // Found a free chunk that is large enough
            
            // Split the chunk if needed and update metadata
            if( splitChunk( currentChunk , roundedSize ) ){
                currentChunk -> is_allocated = 1;  // Mark the chunk as allocated
                return ( void * )( currentChunk -> data );  // Return a pointer to the payload
            }
        }
        // Move to the next chunk
        currentChunk = getNextChunk( currentChunk );
    }
    // If no suitable chunk is found, return NULL
    return NULL;
}

void myfree( void *ptr , char *file , int line ) {
    // Implement free logic here
    // Check if the given pointer is valid
    if ( ptr == NULL ) {
        fprintf( stderr , "Error: Attempt to free a NULL pointer (file: %s, line: %d )\n" , file , line );
        return;
    }

    if( ( char * )ptr - sizeof( Chunk ) < (char *)memory || (char * )ptr >= (char *)memory + MEMLENGTH ){
        fprintf( stderr , "Error: Attempt to free non-heap pointer (file: %s, line: %d )\n" , file , line );
        return;
    }
    
    // Convert the pointer to the corresponding chunk
    Chunk *currentChunk = ( Chunk * )ptr - 1;
    
    if( currentChunk -> is_allocated == 0 ){ // trying to free smth already free 
        fprintf( stderr , "Error: Attempt to free a block that is already been freed (file: %s, line: %d )\n" , file , line );
        return;
    }

    if( currentChunk -> is_allocated != 1 ){ // trying to free an invalid pointer
        fprintf( stderr , "Error: Attempt to free an invalid pointer (file: %s, line: %d )\n" , file , line );
        return;
    }
    // Mark the chunk as free
    currentChunk -> is_allocated = 0;

    // Coalesce adjacent free chunks
    // Check the next chunk
    Chunk *nextChunk = getNextChunk( currentChunk );
    if ( nextChunk != NULL && !nextChunk -> is_allocated ) {
        // Coalesce with the next free chunk
        currentChunk -> size += nextChunk -> size + sizeof( Chunk );
       // nextChunk -> size = 0;  // Mark the next chunk as unused
    }

    // // Check the previous chunk
    // Chunk *prevChunk = ( Chunk * )( currentChunk -> data - sizeof( Chunk ) );
    // if ( prevChunk -> size > 0 && !prevChunk -> is_allocated ) {
    //     // Coalesce with the previous free chunk
    //     prevChunk -> size += currentChunk -> size;
    //     currentChunk -> size = 0;  // Mark the current chunk as unused
    // }
}

// Function to coalesce all adjacent free chunks
void coalesceAll() {
    Chunk *currentChunk = ( Chunk * )memory;
    while ( currentChunk != NULL ) {
        if ( !currentChunk -> is_allocated ) {
            // Check the next chunk
            Chunk *nextChunk = getNextChunk( currentChunk );
            if ( nextChunk != NULL && !nextChunk -> is_allocated ) {
                // Coalesce with the next free chunk
                currentChunk -> size += nextChunk -> size;
                nextChunk -> size = 0;  // Mark the next chunk as unused
            }
        }
        // Move to the next chunk
        currentChunk = getNextChunk( currentChunk );
    }
}

// Function to print the current state of memory
void printMemoryState() {
    printf( "Memory State:\n" );
    Chunk *currentChunk = ( Chunk * )memory;
    while ( currentChunk != NULL ) {
        printf( "Chunk at %p, size: %zu, allocated: %s\n", ( void * )currentChunk , currentChunk -> size , currentChunk -> is_allocated ? "Yes" : "No" );

        // Move to the next chunk
        currentChunk = getNextChunk( currentChunk );
    }
    printf( "\n" );
}

// Function to check for memory leaks
int checkMemoryLeaks() {
    // Implement logic to check if any memory chunks remain allocated
    // Return 1 if there are memory leaks, 0 otherwise
    // Iterate through the memory array to check for allocated chunks
    Chunk *currentChunk = ( Chunk * )memory;
    while ( currentChunk != NULL ) {
        if ( currentChunk -> is_allocated ) {
            // Found an allocated chunk, indicating a memory leak
            printf( "Memory leak detected at address %p, size: %zu\n" , ( void * )currentChunk , currentChunk -> size );
            return 1; // Memory leak found
        }

        // Move to the next chunk
        currentChunk = getNextChunk( currentChunk );
    }

    // No memory leaks found
    return 0;
}

// Function for testing purposes
void testMallocFree() {
    // Implement a set of test cases for malloc and free
    // You can use assert statements to check the correctness of your implementation

    // Test Case 1: Basic Allocation and Free
    void *ptr1 = mymalloc( 16 , __FILE__ , __LINE__ );
    assert( ptr1 != NULL );
    myfree( ptr1 , __FILE__, __LINE__);
    assert( ( ( Chunk * )ptr1 - 1 ) -> is_allocated == 0 );

    // Test Case 2: Double Free - Should Print Error Message
    myfree( ptr1 , __FILE__ , __LINE__ );

    // Test Case 3: Allocate Multiple Chunks
    void *ptr2 = mymalloc( 32 , __FILE__ , __LINE__ );
    void *ptr3 = mymalloc( 24 , __FILE__ , __LINE__ );
    assert( ptr2 != NULL && ptr3 != NULL );

    // Test Case 4: Free Chunks and Coalesce
    myfree( ptr2 , __FILE__ , __LINE__ );
    myfree( ptr3 , __FILE__ , __LINE__ );
    assert( ( ( Chunk * )ptr2 - 1 ) -> is_allocated == 0 && ( ( Chunk * )ptr3 - 1 ) -> is_allocated == 0 );

    // Test Case 5: Allocate Larger Chunk
    void *ptr4 = mymalloc( 64 , __FILE__ , __LINE__ );
    assert( ptr4 != NULL );

    // Test Case 6: Allocate and Free, Check Memory State
    void *ptr5 = mymalloc( 48 , __FILE__ , __LINE__ );
    assert( ptr5 != NULL );
    myfree( ptr4 , __FILE__ , __LINE__ );
    assert( ( ( Chunk * )ptr4 - 1 ) -> is_allocated == 0 );

    // Test Case 7: Check for Memory Leaks
    assert( checkMemoryLeaks() == 0 );

    // Additional Test Case 8: Attempt to Allocate More Than Available Memory
    void *ptr6 = mymalloc( MEMLENGTH * sizeof(double) , __FILE__ , __LINE__ );
    assert( ptr6 == NULL );

    // Additional Test Case 9: Allocate and Free Multiple Times, Check Coalescing
    void *ptr7 = mymalloc( 20 , __FILE__ , __LINE__ );
    void *ptr8 = mymalloc( 15 , __FILE__ , __LINE__ );
    myfree( ptr7 , __FILE__ , __LINE__ );
    myfree( ptr8 , __FILE__ , __LINE__ );
    assert( ( ( Chunk * )ptr7 - 1 ) -> is_allocated == 0 && ( ( Chunk * )ptr8 - 1 ) -> is_allocated == 0 );

    // Additional Test Case 10: Allocate and Free, Check Coalescing
    void *ptr9 = mymalloc( 40 , __FILE__ , __LINE__ );
    void *ptr10 = mymalloc( 20 , __FILE__ , __LINE__ );
    myfree( ptr9 , __FILE__ , __LINE__ );
    myfree( ptr10 , __FILE__ , __LINE__ );
    assert( ( ( Chunk * )ptr9 - 1 ) -> is_allocated == 0 && ( ( Chunk * )ptr10 - 1 ) -> is_allocated == 0 );

    // Add more test cases as needed

    // You can use assert statements to check various conditions and ensure
    // that your memory allocator behaves correctly in different scenarios.

}