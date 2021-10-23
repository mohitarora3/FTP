#include <iostream>
#include <stdlib.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<sstream>
#include<sys/sendfile.h>
#include <fstream>
#include<fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define GetCurrentDir getcwd

using namespace std;

#define MAXLINE 4096 /*max text line length*/


int main(int argc, char **argv)
{
 char response[MAXLINE];
 char *f;
 string F;
 int filehandle,sockfd,size,i;
 struct sockaddr_in servaddr;
 struct stat obj;
 char sendline[MAXLINE], recvline[MAXLINE];
 cout<<"\n---------------------------------------------------";
 cout<<"\n     FTP APPLICATION BETWEEN CLIENT AND SERVER     ";
 cout<<"\n----------------------------------------------------\n\n";
 ;
 cout<<"\n    USER – This command sends the user identification to the server.\n\
    PASS – This command sends the user password to the server.\n\
    MKD – This command causes the directory specified in the path name to be created as a directory.\n\
    CWD – This command allows the user to change the current working directory to the path specified with the command.\n\
    RMD – This command causes the directory specified in the path-name to be removed as a directory.\n\
    PWD – This command causes the name of the current working directory to be returned in the reply.\n\
    RETR – This command send the requested file over the data connection.\n\
    STOR – This command causes to store a file into the current directory of the remote host.\n\
    LIST – Sends a request to display the list of all the files present in the directory.\n\
    ABOR – This command tells the server to abort the previous FTP service command and any associated transfer of data.\n\
    QUIT – This command terminates a USER and if file transfer is not in progress, the server closes the connection.\n\n";

 //basic response of the arguments
 //additional responses can be inserted
 if (argc !=3) 
 {
  cerr<<"Usage: ./a.out <IP address of the server> <port number>"<<endl;
  exit(1);
 }

 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
 {
  cerr<<"Problem in creating the socket"<<endl;
  exit(2);
 }

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(atoi(argv[2])); //convert to big-endian order

 //Connection of the client to the socket
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) 
 {
  cerr<<"Problem in connecting to the server"<<endl;
  exit(3);
 }
 recv(sockfd,response,MAXLINE,0);
 cout<<response;


 cout<<"FTP>";

 while (fgets(sendline, MAXLINE, stdin) != NULL)
  {
  memset(response,0,sizeof(response));
  char* word,*filename;
  string word1,word2;
  char *str,*buf;
  str=sendline;
  stringstream ss(str);
  ss>>word1;
  ss>>word2;
  word=const_cast<char *>(word1.c_str());

   if (strcmp("QUIT\n",sendline)==0) 
    {

   		send(sockfd, sendline, MAXLINE, 0);
   		close(sockfd);
		return 0;
   	}
   else if (strcmp("ABORT\n",sendline)==0)
   {

   		send(sockfd, sendline, MAXLINE, 0);
   		recv(sockfd,response,MAXLINE,0);
   		cout<<response;

   } 
   else if (strcmp("USER",word)==0)  
   {

   	//cout<<endl<<sendline<<endl;
   	send(sockfd, sendline, MAXLINE, 0);
	recv(sockfd,response,MAXLINE,0);
	if(strcmp("0",response)==0)
	{
		cerr<<"\nIvalid username try again"<<endl;
	}
	else
		cout<<response;
	}

	else if (strcmp("PASS",word)==0)  
	{

   	send(sockfd, sendline, MAXLINE, 0);
	recv(sockfd,response,MAXLINE,0);
	cout<<response;
	}

   else if (strcmp("LIST\n",sendline)==0)  
   	{	   

   		strtok(sendline,"\n");
   		send(sockfd, sendline, MAXLINE, 0);
		recv(sockfd, response, MAXLINE, 0);
       if(strcmp(response,"530")==0)
		{
			cout<<"530, Not logged in\n";
		}
		else
		{
			//cout<<"response: "<<response<<endl;
			size=atoi(response);
	    	
	        filehandle = open("t.txt", O_CREAT | O_EXCL | O_WRONLY, 0666);
	        system("cat t.txt");
	       
         }
	}
	
  
   else if (strcmp("PWD\n",sendline)==0) 
    {

   	send(sockfd, sendline, MAXLINE, 0);
	recv(sockfd, response, MAXLINE,0);
	if(strcmp(response,"530")==0)
	{
			cout<<"530, Not logged in\n";
	}
	else
	cout<<response<<endl;
   }

 
   else if (strcmp("CWD",word)==0) 
    {
	
   	//cout<<"Path given is: "<<word<<endl;
	send(sockfd, sendline, MAXLINE, 0);
	recv(sockfd,response,MAXLINE,0);
	if(strcmp(response,"530")==0)
	{
			cout<<"530, Not logged in\n";
	}

	else if(strcmp("0",response)==0)
	{
		cerr<<"Directory doesn't exist. response Path"<<endl;
	}
	else
		cout<<response;
	
   }
     else if(strcmp("MKD",word)==0)
     {
		send(sockfd, sendline, MAXLINE, 0);
		recv(sockfd,response,MAXLINE,0);
		if(strcmp(response,"530")==0)
		{
			cout<<"530 Not logged in\n";
		}
		else
			cout<<response<<endl;
	}
     else if(strcmp("RMD",word)==0)
     {

     	send(sockfd, sendline, MAXLINE, 0);
		recv(sockfd,response,MAXLINE,0);	
		if(strcmp(response,"530")==0)
		{
			cout<<"530 Not logged in\n";
		}
		else
		cout<<response<<endl;

	}
   
 
   else if (strcmp("STOR",word)==0)  
   {

	   	int status;
	   	filename=const_cast<char *>(word2.c_str());
	   	//cout<<filename<<endl;
		filehandle = open(filename, O_RDONLY);
		//cout<<"file handle is :"<<filehandle<<endl;
	    if(filehandle == -1)
	     {
	           cout<<"No such file on the local directory\n";
	           
	     }
	     else
	     {
	     
	     	send(sockfd, sendline, MAXLINE, 0);
			stat(filename, &obj);
		  	size = obj.st_size;
		  	send(sockfd, &size, sizeof(int), 0);
		  	sendfile(sockfd, filehandle, NULL, size);
		  	recv(sockfd,response,MAXLINE, 0);
		  	if(strcmp(response,"0")==0)
		    	cout<<"File failed to be stored to remote machine\n";
		  	else
		    	cout<<"File stored successfully\n";
		 }
	}
   
   else if (strcmp("RETR",word)==0)  
   { 

   		send(sockfd, sendline, MAXLINE, 0);
   		filename=const_cast<char *>(word2.c_str());
		recv(sockfd, response, MAXLINE, 0);
		cout<<"\n"<<response<<"\n";
	  	if(strcmp(response,"530")==0)
		{
			cout<<"530 Not logged in\n";
		}
		else if(strcmp(response,"0")==0)
	    {
	      cout<<"No such file on the remote directory\n";
	  
	    }
	    else
	    {
	    size=atoi(response);
	    //cout<<"size is:  "<<size;
		  f = (char *)malloc(size);
		  recv(sockfd, f, size, 0);
		  while(1)
		    {
		      filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
		      if(filehandle == -1)
		     	{
			     //sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
		   	  		sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
		   	  
		   	  	}
		      else break;
		    }
		  write(filehandle, f, size);
		  close(filehandle);
		  F="cat "+string(filename);
		  //cout<<F<<endl<<F.c_str();
		  system(F.c_str());
		}
	}
   else
   {
   	send(sockfd, sendline, MAXLINE, 0);
	recv(sockfd, response, MAXLINE,0);
   	if(strcmp(response,"530")==0)
	{
			cout<<"530 Not logged in\n";
	}
	else
	cout<<response<<endl;
   }
	cout<<"FTP>";
}

 exit(0);
}
