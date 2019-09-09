#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h> 
#include <ctype.h>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <ctime>
#include<arpa/inet.h>
using namespace std;

void error(const char *err)
{
	perror(err);
	exit(0);
}
void  downloadfile(char * ,char * ,char *);
map< string ,vector<pair<string,string> > > map1;
struct sockaddr_in dserv_addr, dcli_addr;
void downloadserver();
void dostuff (int sock);
int main(int argc, char *argv[])
{
	int pid;
	pid = fork();
	if (pid < 0)
		error("ERROR on fork");
	if (pid == 0)  {
		downloadserver();
	}

	while(1)
	{
		char file[256],path[256],ip[256];
		int sockfd, portno, n,status,j,repo_fd;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		char buffer[256];

		if (argc < 3)
		{
			fprintf(stderr,"wrong input %s\n", argv[0]);
			exit(0);
		}

		portno = atoi(argv[2]);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		if (sockfd < 0) 
			error("ERROR opening socket");

		server = gethostbyname(argv[1]);

		if (server == NULL) 
		{
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
		serv_addr.sin_port = htons(portno);

		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
			error("ERROR connecting");


		//reading menu


		n = read(sockfd,buffer,255);

		if (n < 0) 
			error("ERROR reading from socket");

		printf("%s\n",buffer);


		//writing option
		bzero(buffer,256);
		fgets(buffer,255,stdin);
		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) 
			error("ERROR writing to socket");

		///////client wants to share
		if(buffer[0]=='2')
		{
			//waiting for server response
			bzero(buffer,256);
			status=0;

			while(status==0)
			{

				n = read(sockfd,buffer,255);
				if (n < 0) 
					error("ERROR reading from socket");
				if(buffer[0]!=0)
					status=1;
			}

			if(buffer[0]==-1)
			{
				close(sockfd);
				return 0;
			}


			///sharing file
			printf("%s\n",buffer);
			bzero(buffer,256);
			fgets(buffer,255,stdin);
			n = write(sockfd,buffer,strlen(buffer));

			//final response
			bzero(buffer,256);
			status=0;
			while(status==0)
			{

				n = read(sockfd,buffer,255);
				if (n < 0) 
					error("ERROR reading from socket");
				if(buffer[0]!=0)
					status=1;
			}

			if(buffer[0]==-1)
			{
				close(sockfd);
				return 0;
			}
			printf("%s\n",buffer);
			close(sockfd);
		}

		//client wants to serach
		else if(buffer[0]=='1')
		{
			//waiting for server response
			bzero(buffer,256);
			status=0;

			while(status==0)
			{

				n = read(sockfd,buffer,255);
				if (n < 0) 
					error("ERROR reading from socket");
				if(buffer[0]!=0)
					status=1;
			}

			if(buffer[0]==-1)
			{
				close(sockfd);
				return 0;
			}


			///searching file
			printf("%s\n",buffer);
			bzero(buffer,256);
			fgets(buffer,255,stdin);
			n = write(sockfd,buffer,strlen(buffer));

			//waiting for final response
			bzero(buffer,256);
			status=0;
			while(status==0)
			{

				n = read(sockfd,buffer,255);
				if (n < 0) 
					error("ERROR reading from socket");
				if(buffer[0]!=0)
					status=1;
			}

			if(buffer[0]==-1)
			{
				close(sockfd);
				return 0;
			}

			///fial resonse with mrros in bffer
			int i,k,hshct;
			i=0;
			k=0;
			hshct=0;
			while(buffer[i]!='\0')
			{
				if((buffer[i]!='#' && buffer[i]!='\n') && hshct==0)
				{
					file[k]=buffer[i];
					k++;
				}
				else if((buffer[i]!='#' && buffer[i]!='\n') && hshct==1)
				{
					path[k]=buffer[i];
					k++;
				}
				else if((buffer[i]!='#' && buffer[i]!='\n') && hshct==2)
				{
					ip[k]=buffer[i];
					k++;
				}
				else if(buffer[i]=='\n' && hshct==2)
				{

					hshct=0;
					ip[k]='\0';
					map1[file].push_back(make_pair(path,ip));
					k=0;

				}
				else if(buffer[i]=='#')
				{

					if(hshct==0)
					{
						hshct++;
						file[k]='\0';
					}
					else if (hshct==1)
					{
						hshct=2;
						path[k]='\0';
					}

					k=0;
				}
				i++;

			}
			i=0;
			bzero(buffer,256);
			string  str(file);
			if(map1.find(str)==map1.end())
			{
				cout<<"FILE NOT FOUND\n" ;
				close(sockfd);
			}
			else
			{
				for (std::vector<pair<string,string> >::iterator it = (map1.find(str)->second).begin() ; it != (map1.find(str)->second).end(); ++it)		
				{
					printf("%d. ",i+1);
					printf("%s ",file);
					cout<<it->second<<'\n';
					i++;
				}
				int x=0;
				printf("PLEASE ENtER THE MIRROR YOU WANT TO SELECT\n >>");
				while(x<1  || x>i+1)
				{
					scanf("%d",&x);
					if(x<1  || x>i+1)
						printf("WRONG CHOICE EnTER AGAIN\n>>");
				}
				bzero(path,256);
				bzero(ip,256);
				i=1;
				for (std::vector<pair<string,string> >::iterator it = (map1.find(str)->second).begin() ; it != (map1.find(str)->second).end(); ++it)		
				{
					j=0;
					while(it->first[j]!='\0')
					{
						path[j]=it->first[j];
						j++;
					}
					path[j]='\0';
					j=0;
					while(it->second[j]!='\0')
					{
						ip[j]=it->second[j];
						j++;
					}
					ip[j]='\0';
					if(i==x)
						break;
					i++;
				}
				close(sockfd);
				downloadfile(file,path,ip);
			}
		}

		else if(buffer[0]=='3')
		{
			close(sockfd);
			return 0;
		}

		else
			close(sockfd);
	}
	return 0;
}

void downloadserver()
{
	int dsockfd, dnewsockfd, dportno, dpid;
	socklen_t dclilen;


	dsockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (dsockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &dserv_addr, sizeof(dserv_addr));
	dportno = 19099;
	dserv_addr.sin_family = AF_INET;
	dserv_addr.sin_addr.s_addr = INADDR_ANY;
	dserv_addr.sin_port = htons(dportno);
	if (bind(dsockfd, (struct sockaddr *) &dserv_addr,sizeof(dserv_addr)) < 0) 
		error("ERROR on binding");
	listen(dsockfd,5);
	dclilen = sizeof(dcli_addr);
	while (1) {
		dnewsockfd = accept(dsockfd, (struct sockaddr *) &dcli_addr, &dclilen);
		if (dnewsockfd < 0) 
			error("ERROR on accept");
		dpid = fork();
		if (dpid < 0)
			error("ERROR on fork");
		if (dpid == 0)  {
			close(dsockfd);
			dostuff(dnewsockfd);
			exit(0);
		}
		else close(dnewsockfd);
	} 
	close(dsockfd);

}


void dostuff (int sock)
{
	int log_fd,iu,ku,nu;
	time_t tym;
	struct tm * tyme;
	char ttm[80],log[256];
	time (&tym);
	tyme = localtime(&tym);
	strftime(ttm,80,"%d-%m-%Y %I:%M:%S",tyme);
	char query[256]="DOWNLOAD REQUEST FROM";
	log_fd=open("./log.txt",O_APPEND | O_RDWR |O_CREAT,0666);
	iu=0;
	while(ttm[iu]!='\0')
	{
		log[iu]=ttm[iu];
		iu++;
	}
	ku=iu;
	iu=0;
	while(query[iu]!='\0')
	{
		log[iu+ku]=query[iu];
		iu++;
	}
	log[iu+ku]='\0';
	nu = write(log_fd,log,iu+ku);
	nu = write(log_fd,inet_ntoa(dcli_addr.sin_addr),strlen(inet_ntoa(dcli_addr.sin_addr)));
	char p='\n';
	nu = write(log_fd,&p,1);
	close(log_fd);
	int n,status,fd,i,j,hshct,k;
	///write menu
	char buffer[256],arr[4],c[2];
	//printf("WAITING FOR CLIENT TO RESPOND\n");


	//watng for resposes
	bzero(buffer,256);
	status=0;
	while(status==0)
	{
		n = read(sock,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		status=1;
		if(buffer[0]!=0)
			status=1;
	}
	//printf("%s",buffer);


	////writing file information shared in repo.txt
	fd=open(buffer,O_APPEND | O_RDWR);
	ssize_t nc;
	nc=read(fd, arr,3);
	arr[nc]='\0';
	///final response
	i=0;
	k=0;
	hshct=0;
	while(arr[i]!='\0')
	{

		if (n < 0) error("ERROR writing to socket");
		i++;
		if(i==3)
		{
			n = write(sock,arr,strlen(arr));
			for(j=0;j<4;j++)
			{
				arr[i]='\0';

			}
			nc=read(fd, arr,3);
			arr[nc]='\0';
			i=0;

		}

	}
	n = write(sock,arr,strlen(arr));

	log_fd=open("./log.txt",O_APPEND | O_RDWR |O_CREAT,0666);
	iu=0;
	while(ttm[iu]!='\0')
	{
		log[iu]=ttm[iu];
		iu++;
	}
	ku=iu;
	iu=0;
	while(query[iu]!='\0')
	{
		log[iu+ku]=query[iu];
		iu++;
	}
	log[iu+ku]='\0';
	nu = write(log_fd,log,iu+ku);
	nu = write(log_fd,inet_ntoa(dcli_addr.sin_addr),strlen(inet_ntoa(dcli_addr.sin_addr)));
	p='\n';
	nu = write(log_fd,&p,1);
	close(log_fd);
}

void  downloadfile(char *file,char *path,char *ip)
{
	int sockfd, portno, n,i,k,status,fd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	fd=open(file,O_APPEND | O_RDWR | O_CREAT,0666);
	char buffer[256],arr[4];
	portno = 19101;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	server = gethostbyname(ip);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	i=0;
	k=0;
	while(path[i]!='\0')
	{
		buffer[k]=path[i];
		k++;
		i++;
	}
	i=0;
	while(file[i]!='\0')
	{
		buffer[k]=file[i];
		k++;
		i++;
	}
	buffer[k]='\0';

	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
	{
		error("ERROR writing to socket");
	}
	n = read(sockfd,buffer,255);
	if (n < 0) error("ERROR reading from socket");
	buffer[n]='\0';
	while(n>0)
	{

		write(fd,buffer,strlen(buffer));
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		buffer[n]='\0';
	}
	close(sockfd);
}
