
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Data Types */
typedef struct
{
   char raum;
   char lampe;
   char knoblauch;
}
state_t;

typedef struct
{
   char op1;
   char op2;
}
operator_t;

/* Knowledge base */
operator_t tuer_list[] =
{
   'a','g',
   'a','b',
   'g','d',
   'g','f',
   'f','e',
   'd','e',
   'c','d',
   'b','c'
};

operator_t ueberleben_list[] = 
{
   'd','l',
   'e','k'
};

operator_t mitnehmen_list[] = 
{
   'f','l',
   'c','k'
};



/* Result list */
state_t state_list[200];
operator_t operation_list[200];

char mitnehmen( char raum )
{
   char ret = 0;
   int i;
   int size = sizeof(mitnehmen_list)/sizeof(mitnehmen_list[0]);
   for( i = 0; i < size; i++ )
   {
      if( mitnehmen_list[i].op1 == raum )
      {
         ret = mitnehmen_list[i].op2;
         break;
      }
   }
   return ret;
}

char ueberleben( char raum )
{
   char ret = 0;
   int i;
   int size = sizeof(ueberleben_list)/sizeof(ueberleben_list[0]);
   for( i = 0; i < size; i++ )
   {
      if( ueberleben_list[i].op1 == raum )
      {
         ret = ueberleben_list[i].op2;
         break;
      }
   }
   return ret;
}

int applicable( operator_t op, state_t st )
{
   if( (op.op1 == st.raum) || (op.op2 == st.raum) )
   {
      return 1;
   }
   return 0;
}

void lead_to( operator_t op, state_t st, state_t *new_state )
{
   char item = 0;
   if( op.op1 == st.raum )
   {
      new_state->raum = op.op2;
   }
   else
   {
      new_state->raum = op.op1;
   }
   if( (item = mitnehmen( new_state->raum )) != 0 )
   {
      if( item == 'l' )
      {
         new_state->lampe = item;
         new_state->knoblauch = st.knoblauch;
      }
      else
      {
         new_state->lampe = st.lampe;
         new_state->knoblauch = item;
      }
   }
   else
   {
      new_state->lampe = st.lampe;
      new_state->knoblauch = st.knoblauch;
   }
}

int member( state_t state )
{
   int n;
   int size = sizeof(state_list)/sizeof(state_list[0]);
   for( n=0; n<size; n++ )
   {
      if( 0 == memcmp( &state_list[n], &state, sizeof(state_t) ) )
      {
         return 1;
      }
   }
   return 0;
}

int allowed( state_t state )
{
   char item = 0;
   if( (item = ueberleben( state.raum )) != 0 )
   {
      if( (item == state.knoblauch) || (item == state.lampe) )
      {
         return 1;
      }
      else
      { 
         return 0;
      }
   }
   return 1;
}


/* problem solving */
void vampir( state_t start, state_t end, int st_index, int op_index, int steps )
{
   //printf( "\nSolve step: %d\n", steps );
   
   if( 0 != memcmp( &start, &end, sizeof(state_t) ) )
   {
      int n;
      int size = sizeof(tuer_list)/sizeof(tuer_list[0]);
      for( n=0; n<size; n++ )
      {
         if( applicable( tuer_list[n], start ) )
         {
            state_t new_state;
            lead_to( tuer_list[n], start, &new_state );

            if( 0 == member( new_state ) )
            {
               if( allowed( new_state ) )
               {
                  state_t empty_state = {0, 0, 0};
                  //printf( "State: %c %c %c \n", start.raum, start.lampe, start.knoblauch );   
                  //printf( "Operator: %c %c \n", tuer_list[n].op1, tuer_list[n].op2 );
                  memcpy( &state_list[st_index], &new_state, sizeof(state_t) );
                  memcpy( &operation_list[op_index], &tuer_list[n], sizeof(operator_t) );
                  vampir( new_state, end, st_index+1, op_index+1, steps+1 );
                  memcpy( &state_list[st_index], &empty_state, sizeof(state_t) );
               }
            }
         }
      }
   }
   else
   {
      int n;
      printf("\n===>Solved in %d Steps \n", st_index);
      printf("State list: \n");
      for(n=0; n<st_index; n++)
      {
         printf("%c %c %c \n", state_list[n].raum, state_list[n].lampe, state_list[n].knoblauch );
      }
      printf("\n");
      printf("Operation list: \n");
      for(n=0; n<op_index; n++)
      {
         printf("%c %c \n", operation_list[n].op1, operation_list[n].op2 );
      }
      /* Find only one solution */
      //exit(0);
      printf("\n\n");
   }
}

void main( void )
{
   state_t start = { 'a', 0, 0 };
   state_t end = { 'e', 'l', 'k' };
      
   memset( state_list, 0, sizeof(state_t)*200 );
   memset( operation_list, 0, sizeof(operator_t)*200 );

   printf("\nSolve... \n\n");

   memcpy( state_list, &start, sizeof(state_t) );
   vampir( start, end, 1, 0, 0);

   return;
}



