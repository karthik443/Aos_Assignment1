#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iomanip>
using namespace std;

bool dirFlag = false;

int fd, fd2;
long long totalRead=0;
long long filesize;

bool isdigitChecker(string blockSize){
    for(char ch:blockSize){
        if(!isdigit(ch))return false;
    }
    return true;
}
void printError(string e){
    cout<<e<<endl;
}
//------------------------------------------function to check if directory is present
bool isDirPresent(const char* filename){
    struct stat st;
    if(stat(filename,&st)==0){
        return true;
    }
    return false;
}

void TotalFileReverse(string filename, size_t chunkSize,int data_st_point, int end,string flagVal,int writeIdx) {
    try {

        off_t localFileSize=filesize;
        if(end!=-2){       // I only traverse till given end of file. if set as -2 I will traverse till end;
            if(end>filesize){
                // cout<<filesize<<" - "<<end<<endl;
                throw runtime_error("Given End is greater than file size.");
            }
            localFileSize = end;
            
        }
//------------------------------------------------------------------------start core logic
        char * buffer = new char[chunkSize+1];
        long long data_end_point = localFileSize;
        long long dataToRead = data_end_point-data_st_point;
        while(dataToRead>0){
            size_t readSize = (dataToRead>=chunkSize)? chunkSize:dataToRead;
             data_end_point -=readSize;
            dataToRead = data_end_point-data_st_point;

            if(lseek(fd,data_end_point,SEEK_SET)==-1){
                throw runtime_error("Error in lseek 2");  
            }
            memset(buffer,0,chunkSize+1);
            int readFilesize = read(fd,buffer,readSize);

//------------------------------------------------------------start buffer reversal
                for(int i=0;i<readFilesize/2;i++){
                        char temp = buffer[i];
                        buffer[i]=buffer[readFilesize-i-1];
                        buffer[readFilesize-i-1]= temp;
                }
//------------------------------------------------------------end buffer reversal 

            int ack = write(fd2,buffer,readFilesize);
            if(ack==-1){
                throw runtime_error("Error in writing new file");  
            }
            totalRead += readFilesize;
            double percent = (double(totalRead*100) / filesize) ;

            cout << "\rProgress: " << fixed << setprecision(2) << percent << "%     " << flush;
           

        }
        
//--------------------------------------------------------------End core logic
    }
   catch (exception& e) {
        cout << "Caught: " << e.what();
    }

}

void readFileInReverseChunks(string filename, size_t chunkSize,int st, int end, int doReverse,string flagVal,int writeIdx) {
    try {

       
        off_t localFileSize=filesize;
        if(end!=-2){
            if(end>filesize){
                    // cout<<filesize<<" - "<<end<<endl;
                    throw runtime_error("Error , Given End is greater than file size.");
            }
            localFileSize = end;
        }
       
        //start core logic

        char * buffer = new char[chunkSize+1];
        long long currptr = st;
        long long remaining_fileData;
        float readSize;

        while(currptr<localFileSize){
            remaining_fileData = localFileSize-currptr;
            readSize = (remaining_fileData>=chunkSize)?chunkSize:remaining_fileData;

           
            //move fd to current chunk piece
            if(lseek(fd,currptr,SEEK_SET)==-1){
               throw runtime_error("Error in lseek 2");  
            }
            
            memset(buffer,0,chunkSize+1);
            
            int readFilesize = read(fd,buffer,readSize);
            currptr+=readFilesize;

            if(doReverse){
                for(int i=0;i<readFilesize/2;i++){
                        char temp = buffer[i];
                        buffer[i]=buffer[readFilesize-i-1];
                        buffer[readFilesize-i-1]= temp;
                }
            }

            int ack = write(fd2,buffer,readFilesize);
            if(ack==-1){
                throw runtime_error("Error in writing new file");  
            }
             totalRead += readFilesize;
             double percent = (double(totalRead*100) / filesize) ;

            cout << "\rProgress: " << fixed << setprecision(2) << percent << "%     " << flush;

        }
        
         //End core logic
    }catch (exception& e) {
        cout << "Exception: " << e.what();
    }

}
int main(int argc, char* argv[]){
    try
    {
       
    if(argc<3){
        throw runtime_error("Insufficient Params");
    }
    string fileName = argv[1];
    string flag = argv[2];
    if(stoi(flag)>2){
        throw runtime_error("Invalid Flag");
    }
    string blockSizeInput ;
    // int chunkSize =  500 * 1024; // 512000
    int chunkSize =  50100;


//----------------------------------------------open and store file descriptors
    fd = open(fileName.c_str(),O_RDONLY);
        if(fd==-1){
            throw runtime_error("Error in opening input file");
    }
    filesize = lseek(fd,0,SEEK_END);
        if(filesize==-1){
            throw runtime_error("Error in Calculating filesize");
        }
        
    if(isDirPresent("Assignment1")){
            if(!dirFlag){
                cout<<"Directory is present"<<endl;
                dirFlag = true;
            }
        }else{
            if (mkdir("Assignment1", 0700) == 0) {
                    cout << "Directory created successfully.\n";
                } else {
                    throw runtime_error("Error in creating directory");
                }
    }
    string newFileName = "Assignment1/"+flag+"_"+fileName;
     fd2 = open(newFileName.c_str(),O_WRONLY| O_CREAT| O_TRUNC,0600);
    if(fd2==-1){
            throw runtime_error("Error in creating/opening new file");
            
    }  
//------------------------------------------------------end to create file descriptors
    totalRead=0;
//------------------------------------------------------- start logic according to flags
    if(flag=="0"){
        if(argc!=4){
             throw runtime_error("Incorrect Input params");
            
        }
         blockSizeInput = argv[3];
        if(!isdigitChecker(blockSizeInput)){
             throw runtime_error("Pass a valid block size");
           
        }
        chunkSize = stoi(blockSizeInput);
        readFileInReverseChunks(fileName,chunkSize,0,-2,true,flag,0);
        if(filesize==0){
            cout<<"Empty File"<<endl;
            cout<<"Progress: 100.00%"<<endl;
        }
    }else if(flag=="1"){
         if(argc!=3){
             throw runtime_error("Incorrect Input params");
        }
        TotalFileReverse(fileName,chunkSize,0,-2,flag,0);
        if(filesize==0){
            cout<<"Empty File"<<endl;
            cout<<"Progress: 100.00%"<<endl;
        }
        
    }else if(flag=="2"){
         if(argc!=5){
             throw runtime_error("Incorrect Input params");
      
        }
        string start = argv[3];
        string ending = argv[4];
        if(!isdigitChecker(start) || !isdigitChecker(ending)){
             throw runtime_error("Provide valid st & end Indices");
        }
        int st = stoi(start);
        int end =stoi(ending);
        if(st>end){
            throw runtime_error("start should be less than or equal to end");
           
        }
            TotalFileReverse(fileName,chunkSize,0,st,flag,0);
            readFileInReverseChunks( fileName,chunkSize,st,end+1,false,flag,st);
            TotalFileReverse(fileName,chunkSize,end+1,-2,flag,end+1);
    }
    
    else{
        throw runtime_error("Invalid Flag provided");
    }

    cout<<"\nExecution Completed."<<endl;
    close(fd);
    close(fd2);
//------------------------------------------------------- End logic according to flags

    }
    catch(const std::exception& e)
    {
        close(fd);
        close(fd2);
        cout<<"Exception: "<<e.what()<<endl;
    }
}