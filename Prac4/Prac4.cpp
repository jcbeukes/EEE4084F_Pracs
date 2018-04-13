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
// Please note:  put your student numbers here !!  <<<< NB!  NB!
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
#include <vector>

/** This is the master node function, describing the operations
    that the master will be doing */
void Master () {
   //! <h3>Local vars</h3>
   // The above outputs a heading to doxygen function entry
   int  j;              //! j: Loop counter
   int get[1];    //! Return Buffer
   int send[3];      //! Send Buffer (Segment Size, start, stop)
   char buff[BUFSIZE];  //! buff: Buffer for transferring message data
   std::vector<char> byteBuff;
   MPI_Status stat;     //! stat: Status of the MPI application

   // Read the input image
   if(!Input.Read("Data/greatwall.jpg")){
      printf("Cannot read image\n");
      return;
   }

   // Allocated RAM for the output image
   if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

   int start, stop, size;

   size = Input.Height/(numprocs-1);

   printf("0: We have %d processors\n", numprocs);
   for(j = 1; j < numprocs; j++) {
      start = j*size;                              //Determine start of segment
      if(j == (numprocs-1)){stop = Input.Height;}  //Determine stop of segment
      else{stop = (j+1)*size;}
      
      byteBuff.resize(stop - start, Input.Width*Input.Components); //Create vector with appropriate size
      
      send[0] = (stop - start)*Input.Width*Input.Components; 
      send[1] = start;
      send[2] = stop;
      
      int k =0;
      int p =0;
      int col = 0;
      for (k = start; k < stop; k+=1){                            //Populate 'byteBuff' vector with relevant data
         for (p = 0;p < Input.Width*Input.Components; p+=1){
            byteBuf[col][p] = Input.Rows[k][p];
         }
         bufCount+=1;
      }
      MPI_Send(send, 3, MPI_INT, j, TAG, MPI_COMM_WORLD);          //Send Dimensions
      MPI_Recv(returnVal, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Response - Prevents blocking
      MPI_Send(byteBuf, send[0], MPI_CHAR, j, TAG, MPI_COMM_WORLD); //Send Data
      MPI_Recv(get, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Resonse
      printf("0: Slave %d started\n", j);
   }
}


//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){
 // Start of "Hello World" example..............................................
 char idstr[32];
 char buff [BUFSIZE];

 MPI_Status stat;

 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
 sprintf(idstr, "Processor %d ", ID);
 strncat(buff, idstr, BUFSIZE-1);
 strncat(buff, "reporting for duty", BUFSIZE-1);

 // send to rank 0 (master):
 MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
 // End of "Hello World" example................................................
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
