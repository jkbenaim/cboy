// int pqueue_init( pqueue_root *root );
// int pqueue_isEmpty( pqueue_root *root );
// int pqueue_peek( pqueue_root *root );
// int pqueue_pop( pqueue_root *root );
// int pqueue_hasNext( pqueue_root *root );
// int pqueue_pass( pqueue_root *root, int time );
// int pqueue_insert( pqueue_root *root, int time, int message );

#include <stdlib.h>
#include "pqueue.h"

int pqueue_init( pqueue_root *root )
{
  pqueue_root *newRoot = calloc(1, sizeof(pqueue_root));
  
  if( newRoot )
  {
    root = newRoot;
    return 0;
  }
  else
  {
    return 1;
  }
}

int pqueue_isEmpty( pqueue_root *root )
{
  return root == NULL || root->first == NULL
}

int pqueue_peek( pqueue_root *root )
{
  if( pqueue_isEmpty( root ) )
    return 0;
  else
    return root->first->message;
}

int pqueue_pop( pqueue_root *root )
{
  int returnValue = pqueue_peek( root );
  
  if( root == NULL )
  {
    // nothing to do
  }
  else
  {
    // root is non-null. do we have an element to pop?
    if( pqueue_isEmpty( root ) )
    {
      // root is non-null but the queue is empty. nothing to do.
    }
    else
    {
      // root is non-null and not empty. pop the first element.
      
    }
  }
  return returnValue;
}