#include <stdio.h>
#define LITTLE_E  1

int main( void )
{
   short int testValue = 0x0001;   
   char *lowB = (char*) &testValue;
   
   if( *lowB == LITTLE_E ) 
       printf("This is a Little Endian platform \n");
   else
       printf("This is a Big Endian platform \n");
       
   return 0;    
}
