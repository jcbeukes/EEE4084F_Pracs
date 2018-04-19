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
//
// This is an adaptition of The "Hello World" example avaiable from
// https://en.wikipedia.org/wiki/Message_Passing_Interface#Example_program
//==============================================================================


/** \mainpage Prac4 Main Page
 *
 * \section intro_sec Introduction
 *
 * The purpose of Prac4 is to learn some basics of MPI coding.
 *
 * Look under the Files tab above to see documentation for particular files
 * in this project that have Doxygen comments.
 */



//---------- STUDENT NUMBERS --------------------------------------------------
//
// BKSJOH009 WHTJON002
//
//-----------------------------------------------------------------------------

/* Note that Doxygen comments are used in this file. */
/** \file Prac4
 *  Prac4 - MPI Main Module
 *  The purpose of this prac is to get a basic introduction to using
 *  the MPI libraries for prallel or cluster-based programming.
 */

// Includes needed for the program


#include "Prac4.h"

/** This is the master node function, describing the operations
    that the master will be doing */
void Master(){
 int  j;          //! j: Loop counter
 int get[1];      //! Simple single entry Return Buffer
 int send[3];     //! Simple 3-entry Send Buffer (Segment Size, X Width, Y Height)
 MPI_Status stat; //! stat: Status of the MPI application

 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return;
 }

 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

 int yDiv = std::floor((float)(Input.Height)/(numprocs-1));
 int yDiv2 = 0;
 int size = Input.Height;
 char buf [yDiv+80][Input.Width * Input.Components]; // Space for four on either side


 for(j = 1; j < numprocs; j++){
   tic();
   if (j == 1){yDiv2 = yDiv+4;}
   else if (j < numprocs-1){yDiv2 = yDiv+8;}
   else {yDiv2 = yDiv+4;}
  send[0] = (j < numprocs-1 ? yDiv2 * Input.Width * Input.Components: (size+4) * Input.Width * Input.Components);
  send[1] = Input.Width * Input.Components;
  send[2] = yDiv2;

  int k =0;
  int p =0;
  int col = 0;


  for (k = yDiv*(j-1)-4; k < yDiv*(j-1)+yDiv2;k+=1){

    for (p = 0;p < Input.Width * Input.Components;p+=1){//<<
        if (k<0 or k>=Input.Height){buf[col][p] = 0;}//<<
        else{buf[col][p] = Input.Rows[k][p];}//<<

    }col+=1;


  }
 
  MPI_Send(send, 3, MPI_INT, j, TAG, MPI_COMM_WORLD); //Send Dimensions
  MPI_Recv(get, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Response - Prevents blocking
  MPI_Send(buf, yDiv2 * Input.Width * Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD); //Send Data
  MPI_Recv(get, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Response
  size-=yDiv;

  printf("0: Slave %d started\n", j);
 }


 for(j = 1; j < numprocs; j++){
   if (j == 1){yDiv2 = yDiv+4;}
   else if (j < numprocs-1){yDiv2 = yDiv+8;}
   else {yDiv2 = yDiv+4;}
  MPI_Recv(buf, yDiv2 * Input.Width * Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat); //Get Result
  printf("0: Slave %d Finished\n", j);
  int k =0;
  int p =0;
  int col = 4;
  for (k = yDiv*(j-1); k < yDiv*(j);k+=1){  //Recombine
    for (p = 0;p < Input.Width * Input.Components;p+=1){
      Output.Rows[k][p] = buf[col][p];

    }col+=1;
  }


  printf("0: Slave %d Reassembled\n", j);
  printf("Time = %lg ms\n", (double)toc()/1e-3);
 }

 if(!Output.Write("Data/Output4.jpg")){
  printf("Cannot write image\n");
  return;
 }
  //! <h3>Output</h3> The file Output.jpg will be created on success to save
 //! the processed output.
}
//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){
 int  dim[3];
 char idstr[32];

 MPI_Status stat;

 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 //int MPI_Send(void *data_to_send, int send_count, MPI_Datatype send_type, int destination_ID, int tag, MPI_Comm comm);
 MPI_Recv(dim, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 char In [dim[2]][dim[1]];
 char Out [dim[2]][dim[1]];
 MPI_Send(dim, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);


 MPI_Recv(In, dim[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);

 MPI_Send(dim, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 //

 int x, y, i, p, k;
 int pixels[81]; //Array holding pixels to sort


 for(y = 0; y < dim[2]; y++){
   for(x = 0; x < dim[1]; x+=1){
     k = 0;
     for(p = (y-4); p < (y+5); p++){
       for(i = (x-12); i < (x+13); i+=3){
        //Populate array and handle boundary cases by using 0's.
        if((p>0) && (p<dim[2]) && (i>0) && (i<dim[1])){
        	pixels[k] = In[p][i];
        }
        else{pixels[k] = 0;}
        k+=1;
      }
    }
    //Sort the array
    std::sort(pixels, pixels + 81);
    Out[y][x] = pixels[40]; //Median value
  }
}




 MPI_Send(Out, dim[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD);

}
//------------------------------------------------------------------------------

/** This is the entry point to the program. */
int main(int argc, char** argv){
 int myid;

 // MPI programs start with MPI_Init
 MPI_Init(&argc, &argv);

 // find out how big the world is
 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

 // and this processes' rank is
 MPI_Comm_rank(MPI_COMM_WORLD, &myid);

 // At this point, all programs are running equivalently, the rank
 // distinguishes the roles of the programs, with
 // rank 0 often used as the "master".
 if(myid == 0) Master();
 else          Slave (myid);

 // MPI programs end with MPI_Finalize
 MPI_Finalize();
 return 0;
}
//------------------------------------------------------------------------------
