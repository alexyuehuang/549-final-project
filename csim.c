#include "cachelab.h"
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>

//initialize variables
int s = 0;
int b = 0;
int E = 0;
int hit = 0;
int singlesize = 0;
int cachesize = 0;
int dirtye = 0;
int dirtya = 0;
int miss = 0;
int evict = 0;
int douref = 0;
int v=0;

//load dirty bytes into memory, update dirtye
void load(int *ptr, int B)
{
    //update byte counts
   dirtye += B;
   dirtya -= B;
   //empty the bytes
   for (int i = 0; i < singlesize; ++i)
   {
      *ptr = 0;
      ptr++;
   }
   return;
}

//fill the ptr with the bits needed
void fill(int *ptr, int valid, int dirty, int max, long tag)
{
   long *longptr;
   //valid byte
   *ptr = valid;
   ptr++;
   //dirty byte
   *ptr = dirty;
   ptr++;
   //LRU byte
   *ptr = max;
   ptr++;
   //tag byte
   longptr = (long *)ptr;
   *longptr = tag;
   return;
}

//go into the memory to find the cache line needed
//change the cache line if needed
void changeCache(char *addr, bool iswrite, int *cache)
{
    //initialize variables
    int B=(1<<b); 
   long addrnum = 0;
   long tag = 0;
   long bitmask = 0;
   int max = 0;
   int min = 2147483647; //max int value
   int *setptr;
   int *eleptr;
   long *longptr;
   int *hitptr=NULL;
   int *empty=NULL;
   int *toEvict=NULL;
   
   
   addrnum = strtol(addr, NULL, 16);
   bitmask = (~((~0) << s)) << b;
   //calculate tag
   tag = (long)((unsigned long)addrnum >> (s + b));
   addrnum = (addrnum & bitmask) >> b;
   //ptr to the set
   setptr = cache + E * singlesize * addrnum;
   
   //check if data is in cache
   for (int i = 0; i < E; ++i)
   {
      eleptr = setptr + singlesize * i;
      //if there are empty lines
      if ((*eleptr) == 0)
      {
         empty = eleptr;
         continue;
      }
      //get the maximum
      eleptr += 2;
      if ((*eleptr) > max)
      {
         max = *eleptr;
      }
      //find out what to evict
      if ((*eleptr) < min)
      {
         min = *eleptr;
         toEvict = eleptr - 2;
      }
      eleptr += 1;
      longptr = (long *)eleptr;
      //find if there's a hit
      if (*longptr == tag)
      {
         hitptr = eleptr - 3;
      }
   }
   
   //if we got a hit
   if (hitptr != NULL)
   {
      //write in hitptr
      if(v){
      printf(" hit");
      fflush(stdout);
      }
      hit++;
      hitptr++;
      if (iswrite==true)
      {
         if((*hitptr )==0){
             *hitptr=1;
             dirtya+=B;
         }
      }
      hitptr++;
      //if we got a double ref
      if ((*hitptr) == max)
      {
          if(v){
         printf("-double_ref");
         fflush(stdout);
          }
         douref++;
      }
      else
      {
         *hitptr = max + 1;
      }
   }
   else
   {
       //if we got a miss
      max++;
      if(v){
      printf(" miss");
      fflush(stdout);
      }
      miss++;
      //if we got a empty line
      if (empty != NULL)
      {
          if(v){
          printf(" empty");
            fflush(stdout);
          }
         if (iswrite==true)
         {
             //fill in the new line
            fill(empty, 1, 1, max, tag);
            //update dirty bit
            dirtya+=B;
         }
         else
         {
            fill(empty, 1, 0, max, tag);
         }
      }
      else
      {
          //in this case we evict a line and then load
          //if something went wrong
         if (toEvict == NULL)
         {
             if(v){
            printf("toEvict is null!");
            fflush(stdout);
             }
            return;
         }
         evict++;
         //if we got a dirty eviction
         if ((*(toEvict + 1)) == 1)
         {
             if(v){
            printf(" dirty-eviction");
            fflush(stdout);
             }
            load(toEvict, B);
         }
         else
         {
             //if we got a regular eviction
             if(v){
            printf(" eviction");
            fflush(stdout);
             }
         }
         //replace line
         if (iswrite==false)
         {
             //load line
            fill(toEvict, 1, 0, max, tag);
         }
         else
         {
            fill(toEvict, 1, 1, max, tag);
            dirtya+=B;
         }
   }
}
   return;
}

int main(int argc, char *argv[])
{
    //initialize local variables
   int h = 0;
   size_t size = 0;
   
   char* buffer=NULL;
   FILE *file;
   int *cache;
   char *t;
   char opt;
   char *endptr;
   char *bufptr;
   char *ope;
   char *addr;
   
   //get input variables
   //differ into cases
   while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
   {
      switch (opt)
      {
      case 'h':
         printf("h selected");
         fflush(stdout);
         h = 1;
         break;
      case 'v':
         printf("v selected");
         fflush(stdout);
         v = 1;
         break;
      case 's':
         s = strtoul(optarg, &endptr, 10);
         if (*optarg == '\0' || *endptr != '\0')
         {
            printf("unable to parse");
            fflush(stdout);
            return -1;
         }
         break;
      case 'E':
         E = strtoul(optarg, &endptr, 10);
         if (*optarg == '\0' || *endptr != '\0')
         {
            printf("unable to parse");
            fflush(stdout);
            return -1;
         }
         break;
      case 'b':
         b = strtoul(optarg, &endptr, 10);
         if (*optarg == '\0' || *endptr != '\0')
         {
            printf("unable to parse");
            fflush(stdout);
            return -1;
         }
         break;
      case 't':
         t = optarg;
         break;
      default:
         printf(" unknownoption");
         fflush(stdout);
         return -1;
      }
   }

   //singlesize:int(valid)+int(dirty)+int(time)+long(tag)+stuffs
   singlesize = 1 + 1 + 1 + 2 + (1<<b);
   //total size of cache
   cachesize = singlesize * E * (1<<s);
   //allocate mamory
   cache = (int *)calloc(cachesize, 4);
  (void)h;

   //open file
   file = fopen((const char *)t, "r");
   //if open went wrong
   if (file == NULL)
   {
      printf("No file found");
      fflush(stdout);
      return 0;
   }
   //while there is a line to read
   while (getline(&buffer, &size, file) != -1)
   {
      //don't do anything if I
      if (*buffer != ' ')
      {
         continue;
      }
      //read components of the line
      bufptr = buffer;
      bufptr++;
      ope = bufptr;
      bufptr += 2;
      addr = bufptr;
    if(v){
      printf("\n The string: %s ", ope);
      fflush(stdout);
    }
      //data modify, load and store
      //data modify
      if ((*ope) == 'M')
      {
          if(v){
         printf(" M ");
         fflush(stdout);
          }
          //M is load and store
         changeCache(addr, false, cache);
         changeCache(addr, true, cache);
      }
      //data load
      if ((*ope) == 'L')
      {
          if(v){
         printf(" L ");
         fflush(stdout);
          }
         changeCache(addr, false, cache);
      }
      //data store
      if ((*ope) == 'S')
      {
          if(v){
         printf(" S "); 
         fflush(stdout);
          }
         changeCache(addr, true, cache);
      }
   }
   //free buffers, close file
   free(buffer);
   fclose(file);
   free(cache);
   //return results
   printSummary(hit, miss, evict, dirtye, dirtya, douref);
   return 0;
}
