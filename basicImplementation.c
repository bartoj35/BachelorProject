#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


#define DEFAULT_CAPACITY 2


typedef struct TUnionFind {
	int *	parents;
	int *	elements;
	int 	capacity;
	int 	size;
} UnionFind;


/*@ predicate \freeable_set { L1 } ( UnionFind * ds ) = ( 
  @		( 
  @			ds != \null && 
  @			\valid ( ds ) 
  @		) ==> (
  @ 		\freeable { L1 } ( ds -> elements ) &&
  @     	\freeable { L1 } ( ds -> parents )
  @     )
  @ );
*/


/*@ predicate \valid_parts ( UnionFind * ds ) = (
  @     ( 
  @			ds != \null && 
  @			\valid ( ds ) 
  @		) ==> (
  @     	ds -> size >= 1 &&
  @			ds -> capacity >= 1 && ds -> capacity >= ds -> size &&
  @		    ds -> elements != \null && \valid ( ds -> elements + ( 0 .. ds -> capacity - 1 ) ) &&
  @     	ds -> parents != \null && \valid ( ds -> parents + ( 0 .. ds -> capacity - 1 ) )
  @     )
  @ );      
*/


/*@ logic integer find { L1 } ( UnionFind * ds, integer i, integer length ) = (
  @		( length >= \at ( ds -> size, L1 ) ) 
  @		? 
  @		-1 
  @		: 
  @		(	
  @			( i == \at ( ds -> parents [ i ], L1 ) ) 
  @			? 
  @			i 
  @			:
  @		 	find { L1 } ( ds, ds -> parents [ i ], length + 1 )
  @		) 
  @ );
  @
  @ predicate \correctly_unioned { L1, L2 } ( UnionFind * ds, integer element1, integer element2 ) = (
  @     \forall integer i; 
  @			0 <= i < \at ( ds -> size, L2 ) ==> 
  @     	(
  @     		( 
  @					( 
  @						find { L1 } ( ds, i, 0 ) != find { L1 } (  ds, element1, 0 ) && 
  @						find { L1 } ( ds, i, 0 ) != find { L1 } ( ds, element2, 0 ) 
  @					) ==> (
  @						find { L1 } ( ds, i, 0 ) == find { L2 } ( ds, i, 0 ) 
  @					)
  @				) || ( 
  @					find { L1 } ( ds, i, 0 ) == find { L2 } ( ds, element1, 0 ) && 
  @					find { L2 } ( ds, i, 0 ) == find { L2 } ( ds, element2, 0 ) 
  @				)
  @     	)
  @ );
  @
  @ predicate \is_acyclic { L1 } ( UnionFind * ds ) = (
  @		ds == \null || 
  @		! \valid ( ds ) || 
  @		\forall integer i; 0 <= i < \at ( ds -> size, L1 ) ==> find ( ds, i, 0 ) != -1
  @ );
*/


/*@
  @ requires \freeable_set { Pre } ( set );
  @ requires \valid_parts ( set );
  @ requires \is_acyclic { Pre } ( set );
  @  
  @ allocates \nothing;
  @
  @ assigns \nothing;
  @
  @ frees \nothing;
  @
  @ ensures \result == \true ==> \exists integer index; 0 <= index < set -> size ==> set -> elements [ index ] == element;  
  @ ensures \result == \false ==> \forall integer index; 0 <= index < set -> size ==> set -> elements [ index ] != element;  
  @
  @ ensures \freeable_set { Post } ( set );
  @ ensures \valid_parts ( set );
  @ ensures \is_acyclic { Post } ( set );
@*/	
bool contains ( int element, UnionFind * set ) {
	/*@
	  @ loop invariant 0 <= i <= set -> size;
	  @
	  @ loop assigns i;
  	  @
	  @ loop variant set -> size - i;
	@*/
	for ( int i = 0; i < set -> size; i ++ ) {
		if ( set -> elements [ i ] == element ) {
			return true;
		}
	}
	return false;
}


/*@
  @ requires set != \null;
  @ requires \valid ( set );
  @
  @ requires \freeable_set { Pre } ( * set );
  @ requires \valid_parts ( * set );
  @ requires \is_acyclic { Pre } ( * set );
  @
  @ behavior no_set:
  @		assumes * set == \null;
  @		
  @		allocates * set;		
  @		allocates ( * set ) -> elements;		
  @		allocates ( * set ) -> parents;
  @
  @		assigns * set;		
  @		assigns ( * set ) -> elements;		
  @		assigns ( * set ) -> elements [ 0 ];	
  @		assigns ( * set ) -> parents;	
  @		assigns ( * set ) -> parents [ 0 ];	
  @		assigns ( * set ) -> capacity;	
  @		assigns ( * set ) -> size;	
  @		
  @		frees \nothing;		
  @		
  @		ensures ( * set ) -> elements [ 0 ] == element;
  @		ensures ( * set ) -> parents [ 0 ] == 0;
  @		ensures \result == 0;
  @		
  @     ensures \freeable_set { Post } ( * set );
  @     ensures \valid_parts ( * set );
  @		ensures \is_acyclic { Post } ( * set );
  @
  @ behavior resize_set:	
  @		assumes * set != \null && \freeable { Pre } ( * set );
  @		assumes ( * set ) -> size >= ( * set ) -> capacity; 
  @     assumes \forall integer index; 0 <= index < ( * set ) -> size ==> ( * set ) -> elements [ index ] != element; 
  @		
  @		allocates * set;		
  @		allocates ( * set ) -> elements;		
  @		allocates ( * set ) -> parents;		
  @	
  @		assigns ( * set ) -> elements;	
  @		assigns ( * set ) -> elements [ 0 .. \old ( ( * set ) -> size ) ];	
  @		assigns ( * set ) -> parents;	
  @		assigns ( * set ) -> parents [ 0 .. \old ( ( * set ) -> size ) ];	
  @		assigns ( * set ) -> capacity;	
  @		assigns ( * set ) -> size;	
  @	
  @		frees ( * set ) -> elements;		
  @		frees ( * set ) -> parents;		
  @	
  @		ensures ( * set ) -> elements [ \old ( ( * set ) -> size ) ] == element;
  @		ensures ( * set ) -> parents [ \old ( ( * set ) -> size ) ] == \old ( ( * set ) -> size );
  @		ensures \result == \old ( ( * set ) -> size );
  @		
  @     ensures \freeable_set { Post } ( * set );
  @     ensures \valid_parts ( * set );
  @		ensures \is_acyclic { Post } ( * set );
  @	
  @ behavior no_resize_set:	
  @		assumes * set != \null && \freeable { Pre } ( * set );
  @		assumes ( * set ) -> capacity < ( * set ) -> size; 
  @     assumes \forall integer index; 0 <= index < ( * set ) -> size ==> ( * set ) -> elements [ index ] != element; 
  @		
  @		allocates \nothing;
  @
  @		assigns ( * set ) -> elements [ \old ( ( * set ) -> size ) ];
  @		assigns ( * set ) -> parents [ \old ( ( * set ) -> size ) ];
  @		assigns ( * set ) -> size;
  @
  @		frees \nothing;
  @
  @		ensures ( * set ) -> elements [ \old ( ( * set ) -> size ) ] == element;
  @		ensures ( * set ) -> parents [ \old ( ( * set ) -> size ) ] == \old ( ( * set ) -> size );
  @		ensures \result == \old ( ( * set ) -> size );
  @
  @     ensures \freeable_set { Post } ( * set );
  @     ensures \valid_parts ( * set );
  @		ensures \is_acyclic { Post } ( * set );
  @
  @ behavior in_set:	
  @		assumes * set != \null && \freeable { Pre } ( * set );
  @     assumes \exists integer index; 0 <= index < ( * set ) -> size ==> ( * set ) -> elements [ index ] == element; 
  @		
  @		allocates \nothing;
  @
  @		assigns \nothing;
  @
  @		frees \nothing;
  @
  @		ensures \result == -1;
  @
  @     ensures \freeable_set { Post } ( * set );
  @     ensures \valid_parts ( * set );
  @		ensures \is_acyclic { Post } ( * set );
  @ 
  @ complete behaviors; 
*/
int makeSet ( int element, UnionFind ** set  ) {
	if ( ( * set ) == NULL ) {
		* set = ( UnionFind * ) malloc ( 1 * sizeof ( UnionFind ) );
		( * set ) -> size = 1;
		( * set ) -> capacity = DEFAULT_CAPACITY;
		( * set ) -> elements = ( int * ) malloc ( DEFAULT_CAPACITY * sizeof ( * ( * set ) -> elements ) );
		( * set ) -> parents = ( int * ) malloc ( DEFAULT_CAPACITY * sizeof ( * ( * set ) -> parents ) );
		( * set ) -> elements [ 0 ] = element;
		( * set ) -> parents [ 0 ] = 0;
		return ( * set ) -> size - 1;
	}
	else {
		if ( contains ( element, * set ) == true ) {
			fprintf ( stderr, "The element is already in set!\n" );
			return -1;
		}

		if ( ( * set ) -> capacity == ( * set ) -> size ) {
			( * set ) -> capacity *= 2;
			( * set ) -> size ++;
			( * set ) -> elements = ( int * ) realloc ( ( * set ) -> elements, ( * set ) -> capacity * sizeof ( * ( * set ) -> elements ) );	
			( * set ) -> parents = ( int * ) realloc ( ( * set ) -> parents, ( * set ) -> capacity * sizeof ( * ( * set ) -> parents ) );	
			
			( * set ) -> elements [ ( *set ) -> size - 1 ] = element;
			( * set ) -> parents [ ( *set ) -> size - 1 ] = ( *set ) -> size - 1;
			return ( * set ) -> size - 1;
		}
		else {
			( * set ) -> size ++;
			( * set ) -> elements [ ( *set ) -> size - 1 ] = element;
			( * set ) -> parents [ ( *set ) -> size - 1 ] = ( *set ) -> size - 1;
			return ( * set ) -> size - 1;
		}
	}
}


/*@
  @ requires \freeable_set { Pre } ( set );
  @ requires \valid_parts ( set );
  @ requires \is_acyclic { Pre } ( set );
  @
  @ behavior valid:
  @		assumes 0 <= elementIndex < set -> size;
  @
  @		allocates \nothing;
  @
  @		assigns \nothing;
  @
  @		frees \nothing;
  @
  @		ensures 0 <= \result < set -> size;
  @		ensures \result == find ( set, elementIndex, 0 );
  @		ensures set -> parents [ \result ] == \result;
  @
  @     ensures \freeable_set { Post } ( set );
  @     ensures \valid_parts ( set );
  @     ensures \is_acyclic { Post } ( set );
  @
  @ behavior not_valid:
  @		assumes elementIndex < 0 || elementIndex >= set -> size;
  @
  @		allocates \nothing;
  @
  @		assigns \nothing;
  @
  @		frees \nothing;
  @
  @		ensures \result == -1;
  @
  @     ensures \freeable_set { Post } ( set );
  @     ensures \valid_parts ( set );
  @     ensures \is_acyclic { Post } ( set );
  @
  @ disjoint behaviors;
@*/
int find ( int elementIndex, UnionFind * set ) {
	if ( elementIndex >= 0 && elementIndex < set -> size ) {
    	int id = set -> parents [ elementIndex ];
		//@ ghost int maxToProcess = set -> size;
		/*@
      	  @ loop invariant id >= 0;
      	  @ loop invariant id < set -> size;
 		  @
      	  @ loop assigns id;
		  @
      	  @ loop variant maxToProcess; 
    	@*/
    	while ( id != set -> parents [ id ] ) {
    		id = set -> parents [ id ];
    		//@ ghost maxToProcess = maxToProcess - 1;
		}

    	return id;
    }
    else {
    	fprintf ( stderr, "Invalid element index!\n" );
    	return -1;
	}
}


/*@
  @ requires \freeable_set { Pre } ( set );
  @ requires \valid_parts ( set );
  @ requires \is_acyclic { Pre } ( set );
  @
  @	behavior valid:
  @		assumes 0 <= elementIndex1 < set -> size;
  @		assumes 0 <= elementIndex2 < set -> size;
  @
  @		allocates \nothing;
  @
  @		assigns set -> parents [ 0 .. set -> size ];	
  @
  @		frees \nothing;
  @
  @		ensures \result == true;
  @
  @     ensures \correctly_unioned { Pre, Post } ( set, elementIndex1, elementIndex2 );
  @     ensures \freeable_set { Post } ( set );
  @     ensures \valid_parts ( set );
  @     ensures \is_acyclic { Post } ( set );
  @
  @	behavior invalid_index:
  @ 	assumes ! ( 0 <= elementIndex1 < set -> size ) || ! ( 0 <= elementIndex2 < set -> size );
  @
  @		allocates \nothing;
  @
  @		assigns \nothing;
  @
  @		frees \nothing;
  @
  @		ensures \result == \false;
  @
  @     ensures \freeable_set { Post } ( set );
  @     ensures \valid_parts ( set );
  @     ensures \is_acyclic { Post } ( set );
  @ 
  @ disjoint behaviors; 
@*/
bool unionSet ( int elementIndex1, int elementIndex2, UnionFind * set ) {
    int firstParent = find ( elementIndex1, set );
	int secondParent = find ( elementIndex2, set );
    
	if ( firstParent == -1 || secondParent == -1 ) {
		if ( firstParent == -1 ) {
        	fprintf ( stderr, "Invalid index for first element!\n" );
		}
        if ( secondParent == -1 ) {
			fprintf ( stderr, "Invalid index for second element!\n" );
		}
		return false;
	}

	if ( firstParent == secondParent ) {
    	return true;
    }
    set -> parents [ secondParent ] = elementIndex1;
    return true;
}


/*@
  @ requires \freeable_set { Pre } ( set );
  @ requires \valid_parts ( set );
  @ requires \is_acyclic { Pre } ( set );
  @
  @	allocates \nothing;
  @
  @ assigns set -> capacity;
  @ assigns set -> elements;
  @ assigns set -> parents;
  @ assigns set -> size;
  @
  @ frees set;
  @ frees set -> elements;
  @ frees set -> parents;
@*/
void freeSet ( UnionFind * set ) {
	free ( set -> parents );
	set -> parents = NULL;
	free ( set -> elements );
	set -> elements = NULL;
	set -> size = 0;
	set -> capacity = 0;
	free ( & ( * set ) );
	set = NULL;
}


/*@
  @	requires \true;
  @
  @ allocates \nothing;
  @
  @	assigns \nothing;
  @
  @ frees \nothing;
  @
  @	ensures \result == 0;
*/
int main ( void ) {
	UnionFind * set = NULL;
	// test adding new element
    makeSet ( 1, & set );

	// test adding new element with capacity
    makeSet ( 2, & set );

	// test adding new element with resize
    makeSet ( 3, & set );
    makeSet ( 4, & set );
    makeSet ( 5, & set );
    makeSet ( 6, & set );

    // test adding element that is already in set
 	makeSet ( 6, & set );
	
	int value = 0;
	// find element on valid index
	find ( 1, set );
	
	// find element on negative index
	find ( -333, set );
	
	// find element on too large index
	find ( 10, set );

	// test union	
	unionSet ( 0, 1, set );
	
	unionSet ( 1, 2, set );
	unionSet ( 3, 0, set );

	find ( 1, set );

	// test union of same sets
	unionSet ( 0, 0, set );
	
	// test union negative left set index
	unionSet ( -1, 0, set );
	
	// test union too large left set index
	unionSet ( 10, 0, set );
	
	// test union negative right set index
	unionSet ( 0, -1, set );

	// test union too large left set index
	unionSet ( 0, 10, set );

	// test free
	freeSet ( set );
	//@ assert 0 == 1;
	return 0;
}
