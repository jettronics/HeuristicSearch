
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Data Types */
typedef struct
{
   char dir;
   int kl;
   int ml;
   int kr;
   int mr;
}
state_t;

typedef struct
{
   char dir;
   int k;
   int m;
}
operator_t;

/* Knowledge base */
operator_t operator_list[] =
{
   'l',2,0,
   'l',1,0,
   'l',0,1,
   'l',0,2,
   'l',1,1,
   'r',2,0,
   'r',1,0,
   'r',0,1,
   'r',0,2,
   'r',1,1
};

/* Result list */
state_t state_list[200];
operator_t operation_list[200];

int applicable( operator_t op, state_t st )
{
   if( op.dir == st.dir )
   {
      if( op.dir == 'l' )
      {
         if( (st.kl >= op.k) && (st.ml >= op.m) )
         {
            return 1;
         }
      }
      else
      {
         if( (st.kr >= op.k) && (st.mr >= op.m) )
         {
            return 1;
         }
      }
   }
   return 0;
}

void lead_to( operator_t op, state_t st, state_t *new_state )
{
   if( op.dir == 'l' )
   {
      new_state->dir = 'r';
      new_state->kl = st.kl - op.k;
      new_state->kr = st.kr + op.k;
      new_state->ml = st.ml - op.m;
      new_state->mr = st.mr + op.m;
   }
   else
   {
      new_state->dir = 'l';
      new_state->kl = st.kl + op.k;
      new_state->kr = st.kr - op.k;
      new_state->ml = st.ml + op.m;
      new_state->mr = st.mr - op.m;
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
   if( ((state.kl <= state.ml) || (state.ml == 0)) &&
       ((state.kr <= state.mr) || (state.mr == 0)) )
   {
      return 1;
   }
   return 0;
}


/* problem solving */
void solve( state_t start, state_t end, int st_index, int op_index, int steps )
{
   //printf( "\nSolve step: %d\n", steps );
   
   if( 0 != memcmp( &start, &end, sizeof(state_t) ) )
   {
      int n;
      int size = sizeof(operator_list)/sizeof(operator_list[0]);
      for( n=0; n<size; n++ )
      {
         if( applicable( operator_list[n], start ) )
         {
            state_t new_state;
            lead_to( operator_list[n], start, &new_state );

            if( 0 == member( new_state ) )
            {
               if( allowed( new_state ) )
               {
                  state_t empty_state = { 0, 0, 0, 0, 0 };
                  //printf( "State: %c %d %d %d %d\n", start.dir, start.kl, start.ml, start.kr, start.mr );   
                  //printf( "Operator: %c %d %d\n", operator_list[n].dir, operator_list[n].k, operator_list[n].m );
                  memcpy( &state_list[st_index], &new_state, sizeof(state_t) );
                  memcpy( &operation_list[op_index], &operator_list[n], sizeof(operator_t) );
                  solve( new_state, end, st_index+1, op_index+1, steps+1 );
                  memcpy( &state_list[st_index], &empty_state, sizeof(state_t) );
               }
            }
         }
      }
   }
   else
   {
      int n;
      printf("\n===>Solved in %d Steps \n", steps);
      printf("State list: \n");
      for(n=0; n<st_index; n++)
      {
         printf("%c %d %d %d %d\n", state_list[n].dir, state_list[n].kl, state_list[n].ml, state_list[n].kr, state_list[n].mr );
      }
      printf("\n");
      printf("Operation list: \n");
      for(n=0; n<op_index; n++)
      {
         printf("%c %d %d\n", operation_list[n].dir, operation_list[n].k, operation_list[n].m );
      }
      /* Find only one solution */
      //exit(0);
      printf("\n\n");
   }
}

void main( void )
{
   state_t start = { 'l', 2, 2, 0, 0 };
   state_t end = { 'r', 0, 0, 2, 2 };
      
   memset( state_list, 0, sizeof(state_t)*200 );
   memset( operation_list, 0, sizeof(operator_t)*200 );

   printf("\nSolve... \n\n");

   memcpy( state_list, &start, sizeof(state_t) );
   solve( start, end, 1, 0, 0);

   return;
}



