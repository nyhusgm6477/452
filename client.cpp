using namespace std;
#include <unistd.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <errno.h>
#include <iostream>
#include <string>
#define MAX 80 
//#define PORT 9035 
#define SA struct sockaddr
//ifconfig - gives you inet: the ip address you use
//10.34.40.33

//do data sending here
void clientFunc(int sockfd, char* fname, char* key, int size) { 
        char *packetSize;
        //char* results;
        char data[size];
        FILE* inputFile;
        int i = 0;
        int packetNum = 0;
        int keyLength = strlen(key);  //this kinda sus idk
        //memcpy(packetSize, &size, sizeof(int));
        //send packet size first so server knows what size to have buffer as
        ssize_t size = write(sockfd, (const char *) &packetSize, sizeof(packetSize));
        
        cout << "SIZE SENT: PACKET SIZE : " << size << endl;
        //opening the file 
        inputFile = fopen(fname, "r");
        if(inputFile != NULL) {
            //cout << "here";
            while(!feof(inputFile)) {
                //cout << "heeere";
                //reading the file
                int membs = fread(data, size, 1, inputFile);
                //encrypt the file with the entered key
                std::cout << std::dec << "Sent encrypted packet #" << packetNum << " - encrypted as ";
                packetNum++;
                while(i < size) { 
                    data[i] = key[i % keyLength] ^ data[i];
                    if(i < 4 || i > size-4) {
                        std::cout << std::hex << static_cast<int>(data[i]) << std::dec;
                        if(i == 3) {
                            cout << " ... ";
                        }
                    }
                    i++;
                }
                cout << "\n";
            //write data over to server
            int stuff = write(sockfd, data, size);
            cout << "SIZE SENT: PACKET SIZE : " << stuff << endl;

            //cout << stuff;
            //if(stuff == -1) {
            //    cout << "not working";
            //    cout << strerror(errno);
            //} else {
            //    cout << "working";
           //}
                i = 0;
            }
            
            cout << "Send success!";
            //TODO print out md5 here
        } else {
            //error
            cout << "Invalid file.";
        }    
} 

void serverFunc(int sockfd, char* fname, char* key) { 
    //char *buff ; 
    //char *packetSize = (char*) malloc(sizeof(char) * 256);
    int packetSize;
    int bytes;
    int size = 12; //for packet size
    int packetNum = 0;
    int keyLength = strlen(key);
    FILE* output;

    cout << "IN FUNCTION" << endl;

    if(strlen(fname) != 0) {  //i think this isn't right
        output = fopen(fname, "w");
    } else {
        cout << "put file stuff here";
    }

    cout << "FILE OPENED! " << endl;

    //capturing packet size as first input
    int numRead = read(sockfd, &packetSize, sizeof(packetSize));
    //converting size to an integer
    //memcpy(&size, packetSize, sizeof(int));
    std::cout << "packet size" << packetSize;
    char buff[size];
    //buff = (char*) malloc(size);
    // infinite loop for chat 
    int i = 0;
    //cout << "here";
    int egg;
    while(egg = read(sockfd, buff, size) != -1) { 
        //cout << egg;
        //bzero(buff, size); 
        //cout << "in here";
		
        if(bytes = read(sockfd, buff, sizeof(buff)) > 0) {
            cout << bytes;
            cout << "Recieved encrypted packet #" << packetNum << " - encrypted as ";
                packetNum++;
                while(i < size) { //maybe need an equal to
                    
                    if(i < 4 || i > size-4) {
                        cout << hex << buff[i];
                    }
                    buff[i] = key[i % keyLength] ^ buff[i];
                    i++;
                }
                
         }   

         fwrite(buff, size, 1, output);
    }
     
   //free(buff);
   //free(packetSize);
   
} 

//put client code here
void clientHandler(char* ip, int port, char* fName, int size, char* key) {
    struct sockaddr_in servaddr, cli; 
    int sockfd, connfd;
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Socket creation failed...\n"); 
        exit(0); 
    } 
    else {
        printf("Socket successfully created..\n"); 
		bzero(&servaddr, sizeof(servaddr)); 
  
		// assign IP, PORT 
		servaddr.sin_family = AF_INET; 
		servaddr.sin_addr.s_addr = inet_addr(ip); 
		servaddr.sin_port = htons(port); 
	}
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else {
        printf("connected to the server..\n"); 
		clientFunc(sockfd, fName, key, size); 
  
		close(sockfd);
	}
}

//put server code here
void serverHandler(char* ip, int port, char* save, char* key) {
    int sockfd, connfd, len; 
    int binds;
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else {
        printf("Socket successfully created..\n"); 
        bzero(&servaddr, sizeof(servaddr)); 
    }
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(ip); 
    servaddr.sin_port = htons(port); 
  
    // Binding newly created socket to given IP and verification 
    if ((binds = bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        cout << strerror(errno);
        exit(0); 
    } else {
        printf("Socket successfully binded..\n"); 
    }
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } else {
        printf("Server listening..\n"); 
         
    }
    len = sizeof(cli);
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&servaddr, (socklen_t*)&len); //uh
    
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } else {
        printf("server acccept the client...\n"); 
    }
    // Function for chatting between client and server 
    serverFunc(connfd, save, key); 
  
    // After chatting close the socket 
    close(sockfd); 
}

int main() { 
    int port, size; 
    char runType[256];
    string ip, fName, key, save;
    char* client = "run client";
    char* server = "run server";
    
    //Wait for user to enter whether they want to run sendfile as a client or server
    cin.getline(runType, 256);

    if(strcmp(runType, client) == 0) {
        //Prompting user for data
        //TODO verify if valid data entered
        cout << "Connect to IP address: ";
        cin >> ip;
        cout << "Port #: ";
        cin >> port;
        cout << "File to be sent: ";
        cin >> fName;
        cout << "Pkt size: ";
        cin >> size;
        cout << "Enter encryption key: ";
        cin >> key;

        //converting the strings to char arrays for C code to handle
        char *ipCopy = new char[ip.length() + 1];
        strcpy(ipCopy, ip.c_str());
        char *fNameCopy = new char[fName.length() + 1];
        strcpy(fNameCopy, fName.c_str());
        char *keyCopy = new char[key.length() + 1];
        strcpy(keyCopy, key.c_str());

        //run client-side code
        clientHandler(ipCopy, port, fNameCopy, size, keyCopy);
    } else if (strcmp(runType, server) == 0) {
        //Prompting user for data
        //TODO verify if valid data entered
        cout << "Connect to IP address:";
        cin >> ip;
        cout << "Port #: ";
        cin >> port;
        cout << "Save file to: ";
        cin >> save;
        cout << "Enter encryption key: ";
        cin >> key;
        
        //converting the strings to char arrays for C code to handle
        char *ipCopy = new char[ip.length() + 1];
        strcpy(ipCopy, ip.c_str());
        char *saveCopy = new char[save.length() + 1];
        strcpy(saveCopy, save.c_str());
        char *keyCopy = new char[key.length() + 1];
        strcpy(keyCopy, key.c_str());

        //run server-side code
        serverHandler(ipCopy, port, saveCopy, keyCopy);
    } else {
        //you dumb
        cout << "Please enter client or server.";
    }

    return 0;		
} 
