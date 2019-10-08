#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include<sys/sendfile.h>
#include<fcntl.h>
#include <unistd.h>
#include <unistd.h>
#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/
using namespace std;
bool AUTHORIZED=0;
bool USERNAME=0;

int main (int argc, char **argv)
{

 	int i,listenfd, size,c,connfd, n,filehandle;
 	string username,password,name;
 	ifstream file;
 	pid_t childpid;
 	char *filename;
  	struct stat obj;
 	socklen_t clilen;
 	char buf[MAXLINE];
 	char str[MAXLINE];
 	char cmd[MAXLINE];
 	struct sockaddr_in cliaddr, servaddr;

	 if (argc !=2) 
	 {						//validating the input
	  cerr<<"Usage: ./a.out <port number>"<<endl;
	  exit(1);
	 }

	 if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0) 
	 {
	  cerr<<"Problem in creating the socket"<<endl;
	  exit(2);
	 }


	 servaddr.sin_family = AF_INET;
	 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	 if(atoi(argv[1])<=1024)
	 {
		cerr<<"Port number must be greater than 1024"<<endl;
		exit(2);
	 }
	 servaddr.sin_port = htons(atoi(argv[1]));

	 //bind the socket
	 bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	 //listen to the socket by creating a connection queue, then wait for clients
	 listen (listenfd, LISTENQ);

	 cout<<"Server waiting for connections."<<endl;

	 for ( ; ; ) 
	 {

	  clilen = sizeof(cliaddr);
	  //accept a connection
	  connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);

	  cout<<"Received request..."<<endl;

	  if ( (childpid = fork ()) == 0 ) 
	  {//if it’s 0, it’s child process

	  cout<<"Child created for dealing with client requests"<<endl;
	  send(connfd,"225, Data connection open; no transfer in progress.\n",MAXLINE,0);
	  //close listening socket
	  close (listenfd);
	  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  
	  {
	  		memset(str,0,sizeof(str));
	  		memset(cmd,0,sizeof(cmd));
	  		if(AUTHORIZED)
		   		cout<<"Command received from user "<<username<<": "<< buf;
		   else
		   		cout<<"Command received from user: "<<buf;

		   cout<<"Processing......\n";
		   char *token,*dummy;
		   dummy=buf;
		   token=strtok(dummy," ");

		   if (strcmp("QUIT\n",token)==0)  
		   {
		   	if(AUTHORIZED)
		   	cout<<username<<" has quit\n";
		   else
		   		cout<<"User has quit\n";
		   	close(connfd);
		   	exit(0);
		   
		   }
		   else if(strcmp("ABORT\n",token)==0)
		   {
		   		USERNAME=0;
		   		AUTHORIZED=0;
		   		send(connfd,"200 Command Okay\nSuccessfully logged out!\n",MAXLINE,0);

		   }

		   else if (strcmp("USER",token)==0) 
		    {
		    	token=strtok(NULL," \n");
				file.open("login_details.txt");
	
				while(file>>username)
				{
					if(strcmp(token,username.c_str())==0)
					{	
						USERNAME=1;
						send(connfd,"\n331, User name okay, need password\n",MAXLINE,0);
						goto finish;
					}
					else
						file>>username;
				}
				send(connfd,"0",MAXLINE,0);
				finish:{
				file.close();
				}
			
			}

			else if (strcmp("PASS",token)==0)  
			{
				if(!USERNAME)
				{
						send(connfd,"503, Bad sequence of commands.\n First tell username then password for successful log in\n",MAXLINE,0);
				}
				else
				{
					token=strtok(NULL," \n");
					file.open("login_details.txt");
					while(file>>name)
					{ 
							if(name==username)
								{
									file>>password;
									if(strcmp(token,password.c_str())==0)
									{
										AUTHORIZED=1;
										send(connfd,"\n200, Command okay\nUser has been successfully logged in\n",MAXLINE,0);
									}
									else
									{
										send(connfd,"\nInvalid password\n",MAXLINE,0);
								    	
								    }
								    file.close();
								    goto done;

								}
								file>>password;
					}
					
				}
				send(connfd,"503, Bad sequence of commands.\n First tell username then password for successful log in\n",MAXLINE,0);
				done:{}
			}

			else if(!AUTHORIZED)
			{
				cout<<"User not authorized\n";
				send(connfd,"530",MAXLINE,0);
				continue;
			}	

		   	else if (strcmp("LIST",buf)==0)
		   	  {
				  
				  /*while((des=readdir(dr))!=NULL)
				  {
				  	strcat(lslist,de->d_name);
				  	strcat(lslist,"\n");
				  }
				  closedir(dir);
				  send(connfd,&lslist,sizeof(lslist),0);*/
				  system("ls >t.txt");
				  i = 0;
				  stat("t.txt",&obj);
				  size = obj.st_size;
				  sprintf(str,"%d",size);
				  send(connfd, str, MAXLINE, 0);				
				  filehandle = open("t.txt", O_RDONLY);
				  sendfile(connfd,filehandle,NULL,size);
				
			   }
			
		   else if (strcmp("PWD\n",buf)==0)  
		   {
		   	char curr_dir[MAXLINE];
		   	char response[MAXLINE]="200, Command Okay\n";
			getcwd(curr_dir,MAXLINE-20);
			strcat(response,curr_dir);
			send(connfd, response, MAXLINE, 0);
		   }

		   else if (strcmp("CWD",token)==0) 
		    {
				token=strtok(NULL," \n");
				//cout<<"Path given is: "<<token<<endl;
				if(chdir(token)<0)
				{
					send(connfd,"0",MAXLINE,0);
				}
				else
				{
					send(connfd,"200, Command Okay\n Successfully updated server current directory.\n",MAXLINE,0);
				}
			 }

		   else if(strcmp("MKD",token)==0)
		   {
		   		token=strtok(NULL," \n");
		   		strcat(cmd,"mkdir -p ");
		   		strcat(cmd,token);
		   		//cout<<"command is  :"<<cmd<<endl;
		   		int i=system(cmd);
		   		//cout<<"i value is:  "<<i<<endl;
		   		if(i)
		   			send(connfd,"\n550, Failed to recreate directory\n",MAXLINE,0);
		   		else
		   			send(connfd,"\n250, Directory successfully created\n",MAXLINE,0);

		  	 }
		   else if(strcmp("RMD",token)==0)
		   {
		   		token=strtok(NULL," \n");
		   		cout<<token<<endl;
		   		strcat(cmd,"rm -r ");
		   		strcat(cmd,token);
		   		//cout<<"command is  :"<<cmd<<endl;
		   		int i=system(cmd);
		   		//cout<<"i value is:  "<<i<<endl;
		   		if(i)
		   			send(connfd,"\n550, Failed to remove directory\n",MAXLINE,0);
		   		else
		   			send(connfd,"\n250, Directory successfully removed\n",MAXLINE,0);
		   }

		   else if (strcmp("STOR",token)==0) 
		    {
		   	   
				  int c = 0, len;
				  char *f;
				  filename=strtok(NULL," \n");
				  //cout<<"filename: "<<filename<<endl;
				  recv(connfd, &size, sizeof(int), 0);
				  //cout<<"size is : "<<size<<endl;
				  i = 1;
				  while(1)
			    	{
			      	filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
			      	if(filehandle == -1)
					{
					  sprintf(filename + strlen(filename), "%d", i);
					}
			      	else
					break;
			    	}
					  f = (char *)malloc(size);
					  recv(connfd, f, size, 0);
					  c = write(filehandle, f, size);
					  sprintf(str,"%d",c);
					  //cout<<str;
					  close(filehandle);
					  send(connfd, str, MAXLINE, 0);
			 }
		      
		   

		   else if (strcmp("RETR",token)==0)
		     {
		     	
			      filename=strtok(NULL," \n");
				  //cout<<"filename : "<<filename<<endl;
				  stat(filename, &obj);
				  filehandle = open(filename, O_RDONLY);
				  //cout<<"file handle is :"<<filehandle<<endl;
				  if(filehandle==-1)
				  	   sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
		   	  
				  else
				  {
				  size = obj.st_size;
				  sprintf(str,"%d",size);
				  //cout<<str;
				  send(connfd, str, MAXLINE, 0);
				  if(size)
				  sendfile(connfd, filehandle, NULL, size);
				  close(filehandle);
				}
			}  
				
			else
			{
			  	send(connfd,"\n502, Command not implemented",MAXLINE,0);
			}
			cout<<"\nDone with request\n";

 	}
 }
 }
 	return 0;
 }


