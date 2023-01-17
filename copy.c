#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
//Print with sprintf and write
//1 is stdout, 2 is stderr

int strtoi(char* str)
{
    return (int) strtol(str, NULL, 10);
}

//copy <infile> <outfile> <blocksize>
int main(int argc, char** argv){
   char writeBuf[400];
   uint32_t* inBuf;
   int32_t inFile;
   int32_t outFile;
   int32_t checkSum; 
   ssize_t errReport;
   ssize_t inErr;

   //Default 1024 bytes for blocksize
   int32_t blockSize = (int32_t) 1024;
   int32_t blockModulo;

   //Permissions for new file
   int32_t newPerms = (int32_t) 0600;

   //Checking general arg count
   if((argc > 4) || (argc < 3)){
      sprintf(writeBuf, "Incorrect arguments! Usage: copy <infile> <outfile> <blocksize>\n");
      write(2, writeBuf, strlen(writeBuf));
      exit(1);
   }
   //Checking blocksize arg
   if(argv[3]){
      //Override default if not null in input
      blockSize = strtoi(argv[3]);
      blockModulo = blockSize % 4;

      //Fix if not divisible by 4
      if(blockModulo != 0){
         sprintf(writeBuf, "Blocksize must be divisible by 4. Rounding up.\n");
         write(2, writeBuf, strlen(writeBuf));
         //Modulo gives remainder, subtract from 4 and then add to get to the nearest divisible
         blockSize = blockSize + (4 - blockModulo);
      }
   }

   //Initialize the array
   inBuf = calloc(blockSize, sizeof(uint32_t));

   //Begin main body of program, after input parsing and checking
   inFile = open(argv[1], O_RDONLY);
   if(inFile == -1){
         sprintf(writeBuf, "An error occured when opening the infile.\n");
         write(2, writeBuf, strlen(writeBuf));
         exit(2);
   }
   
   //Make file for copying into
   outFile = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, newPerms);
   if(outFile == -1){
         sprintf(writeBuf, "An error occured when opening/creating the outfile.\n");
         write(2, writeBuf, strlen(writeBuf));
         exit(3);
   }
   
   int i = 0;
   inErr = blockSize;
   while(inErr == blockSize){
      //Read blocksize bytes
      inErr = read(inFile, inBuf, blockSize);
      if(inErr == -1){
         sprintf(writeBuf, "An error occured when reading the file.\n");
         write(2, writeBuf, strlen(writeBuf));
         exit(4);
      }
      //sprintf(writeBuf, "Bytes scanned: %ld\n", inErr);
      //write(1, writeBuf, strlen(writeBuf));

      //If EOF, just break
      if(inErr == 0){
         break;
      }

      //Write to the output file
      errReport = write(outFile, inBuf, inErr);
      if(errReport == -1){
         sprintf(writeBuf, "An error occured when writing the file.\n");
         write(2, writeBuf, strlen(writeBuf));
         exit(5);
      }
      //sprintf(writeBuf, "Bytes written: %ld\n", errReport);
      //write(1, writeBuf, strlen(writeBuf));
      
      //Zero out parts of array not used this loop
      if(inErr < blockSize){
         for(int x = inErr; x < blockSize; x++){
            ((char*)inBuf)[x] = 0;
         }
      }

      //Fix length of inErr to be divisible by 4
      if((inErr != blockSize) && (inErr != 0)){
         inErr = inErr + (4 - (inErr % 4));
      }

      //Clear checksum of previous loop
      checkSum = 0;

      //Now that the end length is fixed, calculate checksum
      for(int j = 0; j < inErr; j = j + 4){
         checkSum = checkSum ^ (uint32_t) inBuf[j];
      }
      sprintf(writeBuf, "%08x ", checkSum);
      write(1, writeBuf, strlen(writeBuf));

      i++;
   }

   sprintf(writeBuf, "\nThe process completed successfully!\n");
   write(1, writeBuf, strlen(writeBuf));
   return 0;
}
