#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iomanip>

using namespace std;

bool dirFlag = false;
bool fileMatch = true;
int fd, fd2;
long long totalRead=0;
long long filesize;
bool equalSize=true;
bool isdigitChecker(string blockSize){
    for(char ch:blockSize){
        if(!isdigit(ch))return false;
    }
    return true;
}
void printError(string e){
    cout<<e<<endl;
}
void checkPermission(string inputFile,string filename){
    // const char* filename = "newfile";
    struct stat st;
    if (stat(inputFile.c_str(), &st) == 0) {
        mode_t permisson = st.st_mode & 0777;

        cout << "User has read permissions on " << filename << ": " 
             << (permisson & S_IRUSR ? "Yes" : "No") << "\n";
        cout << "User has write permission on " << filename << ": " 
             << (permisson & S_IWUSR ? "Yes" : "No") << "\n";
        cout << "User has execute permission on " << filename << ": " 
             << (permisson & S_IXUSR ? "Yes" : "No") << "\n";

        cout << "Group has read permissions on " << filename << ": " 
             << (permisson & S_IRGRP ? "Yes" : "No") << "\n";
        cout << "Group has write permission on " << filename << ": " 
             << (permisson & S_IWGRP ? "Yes" : "No") << "\n";
        cout << "Group has execute permission on " << filename << ": " 
             << (permisson & S_IXGRP ? "Yes" : "No") << "\n";

        cout << "Others has read permissions on " << filename << ": " 
             << (permisson & S_IROTH ? "Yes" : "No") << "\n";
        cout << "Others has write permission on " << filename << ": " 
             << (permisson & S_IWOTH ? "Yes" : "No") << "\n";
        cout << "Others has execute permission on " << filename << ": " 
             << (permisson & S_IXOTH ? "Yes" : "No") << "\n";
    } 
    else {
        cout<<"Error in checking permissions of "<<filename<<endl;
    }

    
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
        char * buffer2 = new char[chunkSize+1];
        long long data_end_point = localFileSize;
        long long dataToRead = data_end_point-data_st_point;
        while(dataToRead>0){
            size_t readSize = (dataToRead>=chunkSize)? chunkSize:dataToRead;
            totalRead += readSize;
            double percent = (double(totalRead*100) / filesize) ;

           
            data_end_point -=readSize;
            dataToRead = data_end_point-data_st_point;

            if(lseek(fd,data_end_point,SEEK_SET)==-1){
                throw runtime_error("Error in lseek 2");  
            }
            memset(buffer,0,chunkSize+1);
            memset(buffer2,0,chunkSize+1);
            int readFilesize = read(fd,buffer,readSize);
            int readFilesize2 = read(fd2,buffer2,readSize);
//------------------------------------------------------------start buffer comparision
                for(int i=0;i<readFilesize;i++){
                    if(buffer[readFilesize-i-1]!=buffer2[i]){
                        fileMatch = false;
                        delete [] buffer2;
                        delete [] buffer;

                         
                        return;
                    }
                }
//------------------------------------------------------------end buffer comparision 



        }
        // cout<<"Both files are matching"<<endl;
        delete [] buffer2;
        delete [] buffer;
        
//--------------------------------------------------------------End core logic
    }
   catch (exception& e) {
        fileMatch=false;
        cout << "Caught: " << e.what();
    }

}

void readFileInReverseChunks(string filename, size_t chunkSize,int st, int end, int doReverse,string flagVal,int writeIdx) {
    try {
        off_t localFileSize=filesize;
        if(end!=-2){
            if(end>filesize){
                    
                    throw runtime_error("Error , Given End is greater than file size.");
            }
            localFileSize = end;
        }
        //start core logic

        char * buffer = new char[chunkSize+1];
        char * buffer2= new char[chunkSize+1];
        long long currptr = st;
        long long remaining_fileData;
        float readSize;

        while(currptr<localFileSize){
            remaining_fileData = localFileSize-currptr;
            readSize = (remaining_fileData>=chunkSize)?chunkSize:remaining_fileData;

            totalRead += readSize;
             double percent = (double(totalRead*100) / filesize) ;

            

            //move fd to current chunk piece
            if(lseek(fd,currptr,SEEK_SET)==-1){
               throw runtime_error("Error in lseek 2");  
            }
            currptr+=readSize;
            memset(buffer,0,chunkSize+1);
            memset(buffer2,0,chunkSize+1);
            
            int readFilesize = read(fd,buffer,readSize);
            int readFilesize2 = read(fd2,buffer2,readSize);
            if(doReverse){
                for(int i=0;i<readFilesize;i++){
                    if(buffer[readFilesize-i-1]!=buffer2[i]){
                        fileMatch = false;
                        delete [] buffer2;
                        delete [] buffer;
                        return ;
                        // throw runtime_error("Both files are not Matching");  
                    }
                }
            }else{
                for(int i=0;i<readFilesize;i++){
                    if(buffer[i]!=buffer2[i]){
                        fileMatch = false;
                        delete [] buffer2;
                        delete [] buffer;
                        return ;
                        //("Both files are not Matching");  
                    }
                }
            }
        }
        // cout<<"Both files are matching. Hooray!";
        delete [] buffer;
        delete [] buffer2;
        
         //End core logic
    }catch (exception& e) {
        fileMatch=false;
        cout << "Exception: " << e.what();
    }

}
int main(int argc, char* argv[]){
    try
    {
       
    if(argc<5){
        throw runtime_error("Insufficient Params");
    }
    string new_filePath = argv[1];
    string old_filePath = argv[2];
    string directory_name = argv[3];
    string flag = argv[4];
    if(stoi(flag)>2){
        throw runtime_error("Invalid Flag");
    }
    string blockSizeInput = "10240";
    int chunkSize = stoi(blockSizeInput);

//----------------------------------------------open and store file descriptors
    fd = open(old_filePath.c_str(),O_RDONLY);
        if(fd==-1){
            throw runtime_error("Error in opening old file file");
    }
    filesize = lseek(fd,0,SEEK_END);
        if(filesize==-1){
            throw runtime_error("Error in Calculating filesize");
        }
   
    if(isDirPresent(directory_name.c_str())){
        if(!dirFlag){
                // cout<<"Directory is present"<<endl;
                dirFlag = true;
        }else{
            // cout<<"Directory is not created"<<endl;
        }
    }
    
    fd2 = open(new_filePath.c_str(),O_RDONLY);
    if(fd2==-1){
            throw runtime_error("Error in opening new file");
            
    }  
    int filesize2 = lseek(fd2,0,SEEK_END);
    lseek(fd2,0,SEEK_SET);
    if(filesize2==-2){
        throw runtime_error("Error in Calculating filesize");
    }
    // cout<<filesize<<" 1-2 "<<filesize2<<endl;
    if(filesize!=filesize2){
        equalSize=false;
        fileMatch=false;
    }
//------------------------------------------------------end to create file descriptors
    totalRead=0;
//------------------------------------------------------- start logic according to flags
    if(flag=="0"){
        if(argc!=6){
             throw runtime_error("Incorrect Input params");
            
        }
         blockSizeInput = argv[5];
        if(!isdigitChecker(blockSizeInput)){
             throw runtime_error("Pass a valid block size");
        }
        chunkSize = stoi(blockSizeInput);
        if(equalSize){
            readFileInReverseChunks(old_filePath,chunkSize,0,-2,true,flag,0);
        }
        if(filesize==0){
            cout<<"Empty File"<<endl;
            
        }
    }else if(flag=="1"){
         if(argc!=5){
             throw runtime_error("Incorrect Input params");
        }
        if(equalSize){
             TotalFileReverse(old_filePath,chunkSize,0,-2,flag,0);
        }
        if(filesize==0){
            cout<<"Empty File"<<endl;
            
        }
        
    }else if(flag=="2"){
         if(argc!=7){
             throw runtime_error("Incorrect Input params");
      
        }
        string start = argv[5];
        string ending = argv[6];
        if(!isdigitChecker(start) || !isdigitChecker(ending)){
             throw runtime_error("Provide valid st & end Indices");
        }
        int st = stoi(start);
        int end =stoi(ending);
        if(st>end){
            throw runtime_error("start should be less than or equal to end");
           
        }if(equalSize){
            if(fileMatch){
                TotalFileReverse(old_filePath,chunkSize,0,st,flag,0);
            }
            if(fileMatch){
                readFileInReverseChunks( old_filePath,chunkSize,st,end+1,false,flag,st);
            }
            if(fileMatch){
                TotalFileReverse(old_filePath,chunkSize,end+1,-2,flag,end+1);
            }
        }
           
            
            
    }
    
    else{
        throw runtime_error("Invalid Flag provided");
    }
    cout<<"\n";
    cout << "Directory is created : " << (dirFlag? "Yes" : "No") << "\n";
    cout <<"Whether file contents are correctly processed: "<< (fileMatch? "Yes" : "No") << "\n";
    cout<<"Both Files Sizes are Same: "<<(equalSize? "Yes" : "No") << "\n";
    cout<<"\n";
    checkPermission(new_filePath,"newfile");
    cout<<"\n";
    checkPermission(old_filePath,"oldfile");
    cout<<"\n";
    checkPermission(directory_name,"directory");
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