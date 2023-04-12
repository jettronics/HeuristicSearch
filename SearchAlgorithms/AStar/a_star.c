
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


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
   float sum;
   char name;
}
cost_t;

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

/* sorting comparision algorithm */
int compare (const void *a, const void *b)
{
   int ascend = -1;
   if( ((cost_t*)a)->sum > ((cost_t*)b)->sum )
   {
      ascend = 1;
   }
   else
   if( ((cost_t*)a)->sum == ((cost_t*)b)->sum )
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
            p_cost_list[n].sum = effort+guess;
            n++;
         }
      }
   }
   return n;
}

/* route determination */
void a_star_search( char start, char destination, int index, float actual_cost )
{
   if( start != destination )
   {
      int n;
      int found = 0;
      cost_t cost_list[10];
      memset( cost_list, 0, sizeof(cost_t)*10 );

      found = find_children( start, destination, index, cost_list );
      if( found > 0 )
      {
         qsort( cost_list, found, sizeof(cost_t), compare );
         printf( "\nRoute: (index: %d, length: %f)\n", index, actual_cost);
         for( n=0; n<index; n++ )
         {
            printf( "%c ", result_list[n] );
         }

         printf( "\nFound: \n");
         for( n=0; n<found; n++ )
         {
            printf( "%f %c \n", cost_list[n].value, cost_list[n].name );
         }
         for( n=0; n<found; n++ )
         {
            float new_cost;
            new_cost = actual_cost + cost_list[n].value;
            result_list[index] = cost_list[n].name;
            a_star_search( cost_list[n].name, destination, index+1, new_cost);
         }
      }
   }
   else
   {
      int n;
      printf("\n===>Route: %d, Length: %f \n", index, actual_cost);
      for(n=0; n<index; n++)
      {
         printf("%c ", result_list[n]);
      }
      printf("\n\n");
      /* Find only one solution */
      exit(0);
   }
}

void main( void )
{
   memset( result_list, 0, 50 );

   printf("\nA* Search\n\n");

   result_list[0] = 'a';
   a_star_search( result_list[0], 'e', 1, 0.0);

   return;
}



