//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#include "Prac2.h"
//------------------------------------------------------------------------------

// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
 int ID = *((int*)Parameter);

 pthread_mutex_lock(&Mutex);
  printf("Hello from thread %d\n", ID);
 pthread_mutex_unlock(&Mutex);

int start, stop, size;

size = Input.Height/Thread_Count;
start = ID*size;

if(ID == (Thread_Count-1)){stop = Input.Height;}
else					  {stop = (ID+1)*size;}

int x, y, i, p, k;
int pixels[81]; //Array holding pixels to sort

//Iterate over all of the pixels in the image
for(y = start; y < stop; y++){
  for(x = 0; x < Input.Width*Input.Components; x++){
    k = 0;
    for(p = (y-4); p < (y+5); p++){
      for(i = (x-12); i < (x+13); i+=3){
        //Populate array and handle boundary cases by using 0's.
        if((p>0) && (p<Input.Height) && (i>0) && (i<Input.Width*Input.Components)){
        	pixels[k] = Input.Rows[p][i];
        }
        else{pixels[k] = 0;}
        k+=1;
      }
    }
    //Sort the array
    std::sort(pixels, pixels + 81);
    Output.Rows[y][x] = pixels[40]; //Median value
  }
}

 pthread_mutex_lock(&Mutex);
  printf("Thread %d: I QUIT!\n", ID);
 pthread_mutex_unlock(&Mutex);

 return 0;
}
//------------------------------------------------------------------------------

int main(int argc, char** argv){
 int j;

 // Initialise everything that requires initialisation
 tic();
 pthread_mutex_init(&Mutex, 0);

 // Read the input image
 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return -1;
 }

 // Allocated RAM for the output image
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return -2;

 // Median filter ----------------------------
// printf("Start of Median filter code...\n");

// tic();

// int x, y, i, p, k;
// int pixels[81]; //Array holding pixels to sort

// //Iterate over all of the pixels in the image
// for(y = 0; y < Input.Height; y++){
//   for(x = 0; x < Input.Width*Input.Components; x++){
//     k = 0;
//     for(p = (y-4); p < (y+5); p++){
//       for(i = (x-12); i < (x+13); i+=3){
//         //Populate array and handle boundary cases by using 0's.
//         if((p>0) && (p<Input.Height) && (i>0) && (i<Input.Width*Input.Components)){
//         	pixels[k] = Input.Rows[p][i];
//         }
//         else{pixels[k] = 0;}
//         k+=1;
//       }
//     }
//     //Sort the array
//     std::sort(pixels, pixels + 81);
//     Output.Rows[y][x] = pixels[40]; //Median value
//   }
// }

// printf("Time = %lg ms\n", (double)toc()/1e-3);
// printf("End of Median filter code...\n\n");
 // End of example -------------------------------------------------------------

 // Spawn threads...
 int       Thread_ID[Thread_Count]; // Structure to keep the tread ID
 pthread_t Thread   [Thread_Count]; // pThreads structure for thread admin

 for(j = 0; j < Thread_Count; j++){
  Thread_ID[j] = j;
  pthread_create(Thread+j, 0, Thread_Main, Thread_ID+j);
 }

 // Printing stuff is a critical section...
 pthread_mutex_lock(&Mutex);
  printf("Threads created :-)\n");
 pthread_mutex_unlock(&Mutex);

 tic();
 // Wait for threads to finish
 for(j = 0; j < Thread_Count; j++){
  if(pthread_join(Thread[j], 0)){
   pthread_mutex_lock(&Mutex);
    printf("Problem joining thread %d\n", j);
   pthread_mutex_unlock(&Mutex);
  }
 }

 // No more active threads, so no more critical sections required
 printf("All threads have quit\n");
 printf("Time taken for threads to run = %lg ms\n", toc()/1e-3);

 // Write the output image
 if(!Output.Write("Data/Output.jpg")){
  printf("Cannot write image\n");
  return -3;
 }

 // Clean-up
 pthread_mutex_destroy(&Mutex);
 return 0;
}
//------------------------------------------------------------------------------
