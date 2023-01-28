#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEFAULT_CAPACITY 2


typedef struct TDisjointSet {
	int *	parents;
	int * elements;
	int *	sizes;
	int 	capacity;
	int 	size;
} DisjointSet;


bool contains ( int element, DisjointSet * set ) {
    for ( int i = 0; i < set -> size; i ++ ) {
    	if ( set -> elements [ i ] == element ) {
        	return true;
    	}
    }
    return false;
}


int makeSet ( int element, DisjointSet ** set  ) {
	if ( ( * set ) == NULL ) {
		* set = ( DisjointSet * ) malloc ( 1 * sizeof ( DisjointSet ) );
		( * set ) -> size = 1;
		( * set ) -> capacity = DEFAULT_CAPACITY;
		( * set ) -> elements = ( int * ) malloc ( DEFAULT_CAPACITY * sizeof ( * ( * set ) -> elements ) );
		( * set ) -> parents = ( int * ) malloc ( DEFAULT_CAPACITY * sizeof ( * ( * set ) -> parents ) );
		( * set ) -> sizes = ( int * ) malloc ( DEFAULT_CAPACITY * sizeof ( * ( * set ) -> sizes ) );

		( * set ) -> elements [ 0 ] = element;
		( * set ) -> parents [ 0 ] = 0;
		( * set ) -> sizes [ 0 ] = 1;
		return ( * set ) -> size - 1;
	}
	else {
		if ( contains ( element, * set ) == true ) {
			printf ( "The element is already in set!\n" );
			return -1;
		}

		if ( ( * set ) -> capacity == ( * set ) -> size ) {
			( * set ) -> capacity *= 2;
			( * set ) -> size ++;
			( * set ) -> elements = ( int * ) realloc ( ( * set ) -> elements, ( * set ) -> capacity * sizeof ( * ( * set ) -> elements ) );	
			( * set ) -> parents = ( int * ) realloc ( ( * set ) -> parents, ( * set ) -> capacity * sizeof ( * ( * set ) -> parents ) );	
			( * set ) -> sizes = ( int * ) realloc ( ( * set ) -> sizes, ( * set ) -> capacity * sizeof ( * ( * set ) -> sizes ) );	
			
			( * set ) -> elements [ ( *set ) -> size - 1 ] = element;
			( * set ) -> parents [ ( *set ) -> size - 1 ] = ( *set ) -> size - 1;
			( * set ) -> sizes [ ( *set ) -> size - 1 ] = 1;
			return ( * set ) -> size - 1;
		}
		else {
			( * set ) -> size ++;
			( * set ) -> elements [ ( *set ) -> size - 1 ] = element;
			( * set ) -> parents [ ( *set ) -> size - 1 ] = ( *set ) -> size - 1;
			( * set ) -> sizes [ ( *set ) -> size - 1 ] = 1;
			return ( * set ) -> size - 1;
		}
	}
}
 
 
bool find ( int elementIndex, DisjointSet * set, int * setID ) {
    if ( elementIndex >= 0 && elementIndex < set -> size ) {
    	* setID = set -> parents [ elementIndex ];
    	while ( ( * setID ) != set -> parents [ * setID ] ) {
        	* setID = set -> parents [ * setID ];
    	}
    	return true;
    }
    else {
    	printf ( "Invalid element index!\n" );
    	return false;
    }
}


bool swap ( int * first, int * second ) {
    if ( first != NULL && second != NULL ) {
    	int tmp = * first;
	    * first = * second;
    	* second = tmp;
    	return true;
    }
    else {
    	return false;
    }
}


bool unionSet ( int elementIndex1, int elementIndex2, DisjointSet ** set ) {
	if ( elementIndex1 >= 0 && elementIndex1 < ( * set ) -> size && elementIndex2 >= 0 && elementIndex2 < ( * set ) -> size ) {
		int firstParent = 0, secondParent = 0;
		find ( elementIndex1, * set, & firstParent );
		find ( elementIndex2, * set, & secondParent );
		
    if ( firstParent == secondParent ) {
			return true;
		}

		if ( ( * set ) -> sizes [ firstParent ] > ( * set ) -> sizes [ secondParent ] ) {
			swap ( & firstParent, & secondParent );
			swap ( & elementIndex1, & elementIndex2 );
		}

		( * set ) -> parents [ firstParent ] = elementIndex2;
		( * set ) -> sizes [ secondParent ] += ( * set ) -> sizes [ firstParent ];
		return true;
	}

	if ( elementIndex1 < 0 || elementIndex1 >= ( * set ) -> size ) {
		printf ( "Invalid index for first element!\n" );
	}
	
	if ( elementIndex2 < 0 || elementIndex2 >= ( * set ) -> size ) {
		printf ( "Invalid index for second element!\n" );
	}
	return false;
}


void freeSet ( DisjointSet * set ) {
	free ( set -> sizes );
	set -> sizes = NULL;
	free ( set -> parents );
	set -> parents = NULL;
	free ( set -> elements );
	set -> elements = NULL;
	set -> size = 0;
	set -> capacity = 0;
	free ( & ( * set ) );
}


void print ( DisjointSet * set ) {
    for ( int i = 0; i < set -> size; i ++ ) {
    	printf ( "%d: %d, size %d\n", i, set -> parents [ i ], set -> sizes [ i ] );
    }
}


int main ( void ) {
	DisjointSet * set = NULL;

	for ( int i = 1; i <= 6; i ++ ) {
		makeSet ( i, & set );
	}

	int a;
	klee_make_symbolic ( & a, sizeof ( a ), "a" );
    makeSet ( a, & set );
	
	int b;
	klee_make_symbolic ( & b, sizeof ( b ), "b" );
	int value = 0;
	find ( b, set, & value );

	int c, d, e, f, g, h, i, j, k;	
	klee_make_symbolic ( & c, sizeof ( c ), "c" );
	klee_make_symbolic ( & d, sizeof ( d ), "d" );
	klee_make_symbolic ( & e, sizeof ( e ), "e" );
	klee_make_symbolic ( & f, sizeof ( f ), "f" );
	klee_make_symbolic ( & g, sizeof ( g ), "g" );
	klee_make_symbolic ( & h, sizeof ( h ), "h" );
	klee_make_symbolic ( & i, sizeof ( i ), "i" );
	klee_make_symbolic ( & j, sizeof ( j ), "j" );
	klee_make_symbolic ( & k, sizeof ( k ), "k" );
	
	unionSet ( b, c, & set );
	unionSet ( d, e, & set );
	unionSet ( f, g, & set );
	unionSet ( h, i, & set );
	unionSet ( j, k, & set );
	
	int l;
	klee_make_symbolic ( & l, sizeof ( l ), "l" );
	find ( l, set, & value );

	freeSet ( set );
	return 0;
}
