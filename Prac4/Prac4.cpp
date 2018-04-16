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
#include <vector>

/** This is the master node function, describing the operations
    that the master will be doing */
void Master () {
  //! <h3>Local vars</h3>
  // The above outputs a heading to doxygen function entry
  int  j;              //! j: Loop counter
  int get[1];    //! Return Buffer
  int send[3];      //! Send Buffer (Segment Size, start, stop)
  std::vector<char> byteBuff; //! byteBuff: Buffer for transferring message data
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

    send[0] = Input.Height; 
    send[1] = stop - start;
    send[2] = Input.Width*Input.Components;

    int k =0;
    int p =0;
    int col = 0;
    for (k = start; k < stop; k+=1){                            //Populate 'byteBuff' vector with relevant data
      for (p = 0;p < Input.Width*Input.Components; p+=1){
        byteBuff[col][p] = Input.Rows[k][p];
      }
      col+=1;
    }
    MPI_Send(send, 3, MPI_INT, j, TAG, MPI_COMM_WORLD);          //Send Dimensions
    MPI_Recv(get, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Response - Prevents blocking
    MPI_Send(byteBuff, send[0], MPI_CHAR, j, TAG, MPI_COMM_WORLD); //Send Data
    MPI_Recv(get, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Resonse
    printf("0: Slave %d started\n", j);
  }
  for(j = 1; j < numprocs; j++){
    
    start = j*size;                              //Determine start of segment
    if(j == (numprocs-1)){stop = Input.Height;}  //Determine stop of segment
    else{stop = (j+1)*size;}
    byteBuff.resize(stop - start, Input.Width*Input.Components); //Create vector with appropriate size

    MPI_Recv(byteBuf, (stop - start)* Input.Width * Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat); //Get Result
    printf("0: Slave %d Finished\n", j);

    int k =0;
    int p =0;
    int col = 0;
    for (k = start; k < stop; k+=1){  //Recombine
      for (p = 0;p < Input.Width * Input.Components;p+=1){
        Output.Rows[k][p] = byteBuf[col][p];
      }
      col+=1;
    }
    printf("0: Slave %d Reassembled\n", j);
  }
  if(!Output.Write("Data/Output.jpg")){
  printf("Cannot write image\n");
  return;
 }
}


//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){
 // Start of "Hello World" example..............................................
 int dim[3];
 char idstr[32];
 MPI_Status stat;

 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 // Recieve dimensions and total size of data from master.
 // Create matrices of the appropriate size to work with 
 // and send reply to Master
 MPI_Recv(dim, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 char In [dim[1]][dim[2]]; 
 char Out [dim[1]][dim[2]];
 MPI_Send(dim, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);

 // receive from rank 0 (master):
 // receive data to work with.
 //reply to master.
MPI_Recv(In, dim[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);

MPI_Send(dim, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);


int x, y, i, p, k;
int pixels[81]; //Array holding pixels to sort

//Iterate over all of the pixels in the image
for(y = 0; y < dim[1]; y++){
  for(x = 0; x < dim[2]; x++){
    k = 0;
    for(p = (y-4); p < (y+5); p++){
      for(i = (x-12); i < (x+13); i+=3){
        //Populate array and handle boundary cases by using 0's.
        if((p>0) && (p<dim[0]) && (i>0) && (i<dim[2])){
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
 //Return the filtered data to the Master.
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
