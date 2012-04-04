#include "stack.h"


struct stack_list* push(struct stack_list* s, Real* c )
{
  struct stack_real* p = malloc( 1 * sizeof *p );

  if( NULL == p )
    {
      fprintf(stderr, "malloc() failed\n");
      return s;
    }

  p->cur = c;
  p->next = NULL;


  if( NULL == s )
    {
      fprintf(stderr, "Stack not initialized ?\n");
      free(p);
      return s;
    }
  else if( NULL == s->head )
    {
      /*      printf("Stack is Empty, adding first element\n"); */
      s->head = p;
      return s;
    }
  else
    {
      /*      printf("Stack not Empty, adding in front of first element\n"); */
      p->next = s->head;
      s->head = p;  /* push new element onto the head */
    }

  return s;
}


struct stack_list* pop( struct stack_list* s )
{
  struct stack_real* p = NULL;

  if( NULL == s )
    {
      printf("There is no stack list ?\n");
    }
  else if( NULL == s->head )
    {
      printf("There is no element on the stack\n");
    }
  else
    {
      p = s->head;
      s->head = s->head->next;
      free(p);
    }

  return s;
}


struct stack_real* top( struct stack_list* s)
{
  if( NULL == s )
    {
      printf("There is no stack list ?\n");
      return NULL;
    }
  else if( NULL == s->head )
    {
      printf("There is no element on the stack\n");
    }


  return s->head;
}


/* Make a Stack empty */
struct stack_real* make_null( struct stack_list* s )
{
  if( NULL == s )
    {
      printf("Can not make NULL when there is no Stack List\n");
      return NULL;
    }
  else if( NULL == s->head )
    {
      printf("Stack is already Empty\n");
    }
  else
    {
      stack_free(s);
    }

  return s->head;
}


struct stack_real* is_empty( struct stack_list* s )
{
  if( NULL == s )
    {
      printf("There is no Stack\n");
      return NULL;
    }
  else if( NULL == s->head )
    {
      printf("Stack is Empty\n");
    }
  else
    {
      printf("Stack is not Empty\n");
    }

  return s->head;
}



/* ---------- small helper functions -------------------- */
struct stack_list* stack_free( struct stack_list* s )
{
  if( NULL == s )
    {
      printf("Can't free a NULL stack list\n");
    }

  while( s->head ) pop(s);

  return s;
}


struct stack_list* stack_new( void )
{
  struct stack_list* p = malloc( 1 * sizeof *p );

  if( NULL == p )
    {
      fprintf(stderr, "malloc() in Stack Initialization failed\n");
      exit( EXIT_FAILURE );  /* There is no point in going beyond this point */
    }

  p->head = NULL;

  return p;
}


