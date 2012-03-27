/* A stack implementation
 * Author: Haz-Edine Assemlal
 * Date: March 2012
 */

#ifndef DISPLAY_STACK_H
#define DISPLAY_STACK_H

#include "display.h"
#include <stdio.h>
#include <stdlib.h>


struct stack_real
{
	Real* cur;
	struct stack_real* next;
};

struct stack_list
{
	struct stack_real* head;
};

struct stack_real* top( struct stack_list* );
struct stack_real* make_null( struct stack_list* );
struct stack_real* is_empty( struct stack_list* );
struct stack_list* push( struct stack_list*, Real* );
struct stack_list* pop( struct stack_list* );
struct stack_list* stack_new( void );
struct stack_list* stack_free( struct stack_list* );


#endif
