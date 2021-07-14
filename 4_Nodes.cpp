A sequence ring of 4 nodes

#include <iostream>
#include <mpi.h>

//Hash functions=========================================================
int hash1(int ip){
  for(int x = 1; x < 100; x++){
    ip = ip * x % 254;
  }
  return ip;
}

int hash2(int ip){
  for(int x = 1; x < 50; x++){
    ip = ((ip * x) + 2) % 254;
  }
  return ip;
}

//the master process in this group========================================
void master(void) {
  int hashInput = hash1(233) - hash2(422);

  //broadcast the hashInput to all the other processes
  MPI_Send(&hashInput, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  std::cout << "rank 0 has sent information to rank 1" << std::endl;

  //delete the memory for the array;
  delete &hashInput;
}

//the slave processes in this group========================================
void slave(int rank){
  int newHashInput;
  int next = rank + 1;
  int last = rank - 1;

  if(rank == 1){
    int hashInput = 0;
    MPI_Recv(&hashInput, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << "rank" << rank << "has recieved information from rank" <<last << std::endl;
    newHashInput = hash1(hashInput) - hash2(hashInput);
    MPI_Send(&newHashInput, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    std::cout << "rank" << rank << "has sent information to rank"<< next << std::endl;
  }else if( rank == 2){
    MPI_Recv(&newHashInput, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << "rank" << rank << "has recieved information from rank"<< last << std::endl;
    newHashInput = hash1(newHashInput) - hash2(newHashInput);
    MPI_Send(&newHashInput, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    std::cout << "rank" << rank << "has sent information to rank" <<next << std::endl;
  }else if(rank == 3){
    MPI_Recv(&newHashInput, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << "rank" << rank << "has recieved information from rank"<< last << std::endl;
    newHashInput = hash1(newHashInput) - hash2(newHashInput);
    int finalHash1 = hash1(newHashInput);
    int finalHash2 = hash2(newHashInput);
    std::cout << "Final hash1 = " <<finalHash1<<std::endl;
    std::cout << "Final hash2 = " <<finalHash2<<std::endl;
  }

  //delete the memory for the Intergers
  delete &newHashInput;
  delete &next;
  delete &last;
}

//main=======================================================================
int main(int argc, char **argv){
  //initialise MPI
  MPI_Init(NULL, NULL);

  //get the size and rank of the world
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0){
    master();
  }
  else{
    slave(rank);
  }

  //finalise MPI and return control to the OS
  MPI_Finalize();
  return 0;

}
