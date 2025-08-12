#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

using namespace std;

bool dirFlag = false;

int fd, fd2;
bool isdigitChecker(string blockSize){
    for(char ch:blockSize){
        if(!isdigit(ch))return false;
    }
    return true;
}
void printError(string e){
    cout<<e<<endl;
}
//function to check if directory is present
bool isDirPresent(const char* filename){
    struct stat st;
    if(stat(filename,&st)==0){
        return true;
    }
    return false;
}

void TotalFileReverse(string filename, size_t chunkSize,int st, int end,string flagVal,int writeIdx) {
    try {


       
        off_t filesize = lseek(fd,0,SEEK_END);
        if(filesize==-1){
            throw runtime_error("Error in Calculating filesize");
        }
        if(end!=-2){       // I only traverse till specified end of file. if set as -2 I will traverse till end;
            if(end>filesize){
                throw runtime_error("Given End is greater than file size.");
            }
            filesize = end;
            
        }
        
        //start core logic
        char * buffer = new char[chunkSize+1];
        int remaining = filesize;
        int diff = remaining-st;
        while(diff>0){
            size_t readSize = (diff>=chunkSize)? chunkSize:diff;
            remaining -=readSize;
            diff = remaining-st;

            if(lseek(fd,remaining,SEEK_SET)==-1){
                throw runtime_error("Error in lseek 2");  
            }
            memset(buffer,0,chunkSize+1);
            int readFilesize = read(fd,buffer,readSize);
            //start buffer reversal
                for(int i=0;i<readFilesize/2;i++){
                        char temp = buffer[i];
                        buffer[i]=buffer[readFilesize-i-1];
                        buffer[readFilesize-i-1]= temp;
                }
            //end buffer reversal 
            int ack = write(fd2,buffer,readFilesize);
            if(ack==-1){
                throw runtime_error("Error in writing new file");  
            }

        }
        
         //End core logic
    }
   catch (exception& e) {
        cout << "Caught: " << e.what();
    }

}

void readFileInReverseChunks(string filename, size_t chunkSize,int st, int end, int doReverse,string flagVal,int writeIdx) {
    try {

       
        off_t filesize = lseek(fd,0,SEEK_END);
        if(filesize==-1){
            throw runtime_error("Error in Calculating filesize");
        }
        if(end!=-2){
            if(end>filesize){
                    throw runtime_error("Given End is greater than file size.");
            }
            filesize = end;
        }


        //end code to create new file 

        //start core logic

        char * buffer = new char[chunkSize+1];
        int currptr = st;
        int remaining=0;
        int readSize;

        while(currptr<filesize){
            remaining = filesize-currptr;
            readSize = (remaining>=chunkSize)?chunkSize:remaining;
        
            if(lseek(fd,currptr,SEEK_SET)==-1){
               throw runtime_error("Error in lseek 2");  
            }
            currptr+=readSize;
            memset(buffer,0,chunkSize+1);
            
            int readFilesize = read(fd,buffer,readSize);
            cout<<readFilesize<<" -readFilesize"<<endl;
            if(doReverse){
                for(int i=0;i<readFilesize/2;i++){
                        char temp = buffer[i];
                        buffer[i]=buffer[readFilesize-i-1];
                        buffer[readFilesize-i-1]= temp;
                }
            }
            // buffer[readFilesize]='\0';
            int ack = write(fd2,buffer,readFilesize);
            if(ack==-1){
                throw runtime_error("Error in writing new file");  
            }
        }
        
     
         //End core logic
    }catch (exception& e) {
        cout << "Caught: " << e.what();
    }

}
int main(int argc, char* argv[]){
    try
    {
       
    if(argc<3){
        printError("Insufficient Input Params.");
        return 0;
    }
    string fileName = argv[1];
    string flag = argv[2];
    if(stoi(flag)>2){
        throw runtime_error("Invalid Flag");
    }
    string blockSizeInput = "10240";
    int chunkSize = stoi(blockSizeInput);

    // open and store file descriptors
     fd = open(fileName.c_str(),O_RDONLY);
        if(fd==-1){
            throw runtime_error("Error in opening input file");
    }
    if(isDirPresent("Assignment1")){
            if(!dirFlag){
                cout<<"Dir is present";
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

    if(flag=="0"){
        if(argc!=4){
            printError("Incorrect Input params");
            return 0;
        }
         blockSizeInput = argv[3];
        if(!isdigitChecker(blockSizeInput)){
            cout<<"Pass a valid block size";
            return 1;
        }
        chunkSize = stoi(blockSizeInput);
        readFileInReverseChunks(fileName,chunkSize,0,-2,true,flag,0);
    }else if(flag=="1"){
         if(argc!=3){
            printError("Incorrect Input params");
            return 0;
        }
        cout<<"Atleast I'm here"<<endl;
        TotalFileReverse(fileName,chunkSize,0,-2,flag,0);
        
    }else if(flag=="2"){
         if(argc!=5){
            printError("Incorrect Input params");
            return 0;
        }
        string start = argv[3];
        string ending = argv[4];
        if(!isdigitChecker(start) || !isdigitChecker(ending)){
            printError("Provide valid st & end Indices");
            return 0;
        }
        int st = stoi(start);
        int end =stoi(ending);
        if(st>end){
            printError("start should be less than or equal to end");
            return 0;
        }
            TotalFileReverse(fileName,chunkSize,0,st,flag,0);
            readFileInReverseChunks( fileName,chunkSize,st,end+1,false,flag,st);
            TotalFileReverse(fileName,chunkSize,end+1,-2,flag,end+1);
    }
    
    else{
        printError("Invalid Flag provided");
        return 0;
    }
    close(fd);
    close(fd2);
    }
    catch(const std::exception& e)
    {
        close(fd);
        close(fd2);
        cout<<e.what()<<endl;
    }
}