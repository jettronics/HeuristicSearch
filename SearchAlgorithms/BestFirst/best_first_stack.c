
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STACK_SIZE 100

/* Data Types */
typedef struct
{
   char name;
   int x;
   int y;
}
place;

typedef struct
{
   char start;
   char end;
}
street;

typedef struct
{
   float value;
   float heuristic;
   char name;
}
cost_t;

typedef struct
{
   char name;
   int index;
   float cost;
}
stack_elem_t;

/* Stack list */
stack_elem_t stack_list[STACK_SIZE];
int stack_index;

/* Knowledge base */
place place_list[] =
{
   'a',0,2,
   'b',2,0,
   'c',4,2,
   'd',7,0,
   'e',7,3,
   'f',9,2,
   'g',6,5,
   'h',3,5
};

street street_list[] =
{
   'a','b',
   'a','h',
   'a','c',
   'h','c',
   'b','c',
   'c','d',
   'h','g',
   'g','e',
   'e','f',
   'd','f'
};

/* Result list */
char result_list[50];

void clear(void)
{
   memset( stack_list, 0, sizeof(stack_elem_t)*STACK_SIZE );
   stack_index = 0;
   return;
}

void push( stack_elem_t elem )
{
   stack_list[stack_index] = elem;
   stack_index++;
   if( stack_index >= STACK_SIZE )
   {
      stack_index = STACK_SIZE - 1;;
   }
}

stack_elem_t pop( void )
{
   stack_elem_t ret;
   stack_index--;
   if( stack_index < 0 )
   {
      stack_index = 0;
   }
   ret = stack_list[stack_index];
   return ret;
}

/* sorting comparision algorithm */
int compare (const void *a, const void *b)
{
   int ascend = -1;
   if( ((cost_t*)a)->heuristic > ((cost_t*)b)->heuristic )
   {
      ascend = 1;
   }
   else
   if( ((cost_t*)a)->heuristic == ((cost_t*)b)->heuristic )
   {
      ascend = 0;
   }
   return ascend;
}

/* cost calculation */
float cost( char name1, char name2 )
{
   int i;
   int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
   float ret;
   int size = sizeof(place_list)/sizeof(place_list[0]);
   for( i=0; i<size; i++ )
   {
      if( name1 == place_list[i].name )
      {
         x1 = place_list[i].x;
         y1 = place_list[i].y;
      }
      if( name2 == place_list[i].name )
      {
         x2 = place_list[i].x;
         y2 = place_list[i].y;
      }
   }
   ret = sqrt((float)((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)));
   return ret;
}


/* location connection by street */
int applicable( char start, char next, char destination, float *p_cost, float *p_heuristic )
{
   int i;
   int applicable = 0;
   int size = sizeof(street_list)/sizeof(street_list[0]);
   if( start == next )
   {
      return 0;
   }
   for( i=0; i<size; i++ )
   {
      if( ((street_list[i].start == start) || (street_list[i].end == start))
          &&
          ((street_list[i].start == next) || (street_list[i].end == next)) )
      {
         applicable = 1;
         *p_cost = cost( start, next );
         *p_heuristic = cost( next, destination );
         return 1;
      }
   }
   return 0;
}


/* find next nodes implementation */
int find_children( char start, char destination, int index, cost_t *p_cost_list )
{
   int i;
   int n = 0;
   float effort;
   float guess;
   int size = sizeof(place_list)/sizeof(place_list[0]);
   for( i=0; i<size; i++ )
   {
      char next = place_list[i].name;
      if( applicable( start, next, destination, &effort, &guess ) )
      {
         if( NULL == memchr( (char*)result_list, next, index) )
         {
            p_cost_list[n].name = next;
            p_cost_list[n].value = effort;
            p_cost_list[n].heuristic = guess;
            n++;
         }
      }
   }
   return n;
}

/* route determination */
void best_first_search( char start, char destination, int index, float actual_cost )
{
   int n;
   cost_t cost_list[10];
   stack_elem_t element;

   element.name = start;
   element.index = index;
   element.cost = actual_cost;
   push(element);
   
   while( element.name != destination )
   {
      int found = 0;
      
      memset( cost_list, 0, sizeof(cost_t)*10 );
      
      found = find_children( element.name, destination, element.index, cost_list );
      if( found > 0 )
      {
         qsort( cost_list, found, sizeof(cost_t), compare );
         printf( "\nRoute: (index: %d, length: %f)\n", element.index, element.cost);
         for( n=0; n<element.index; n++ )
         {
            printf( "%c ", result_list[n] );
         }

         printf( "\nFound: \n");
         for( n=0; n<found; n++ )
         {
            printf( "%f %c \n", cost_list[n].heuristic, cost_list[n].name );
         }
         for( n=found-1; n>=0; n-- )
         {
            stack_elem_t new_element;
            result_list[element.index] = cost_list[n].name;
            //best_first_search( cost_list[n].name, destination, index+1, new_cost);
            //Push name, index and cost
			new_element.name = cost_list[n].name;
			new_element.cost = element.cost + cost_list[n].value;
			new_element.index = element.index + 1;
			push(new_element);
         }
      }
	  
	  // Pop start, index and cost
	  element = pop();
   }
   
   printf("\n===>Route: %d, Length: %f \n", element.index, element.cost);
   for(n=0; n<element.index; n++)
   {
      printf("%c ", result_list[n]);
   }
   printf("\n\n");
   /* Find only one solution */
   return;
}

void main( void )
{
   memset( result_list, 0, 50 );
   
   clear();

   printf("\nBest First Search\n\n");

   result_list[0] = 'a';
   best_first_search( result_list[0], 'e', 1, 0.0);

   return;
}



