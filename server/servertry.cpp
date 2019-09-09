////working fine fo serach share exit
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <ctype.h>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <ctime>
#include<arpa/inet.h>
using namespace std;
struct sockaddr_in serv_addr, cli_addr;

map< string ,vector<pair<string,string> > > map1;
/**/

void dostuff(int); 
void error(const char *msg)
{
	perror(msg);
	exit(1);
}
void readrepo();

int main(int argc, char *argv[])
{
	char file[256],path[256],ip[256];
	int sockfd, newsockfd, portno, pid,k,m,u,j;
	socklen_t clilen;

	int repo_fd,i,hshct;
	repo_fd=open("./repo.txt",O_RDONLY);
	char buffer[4];


	ssize_t n;
	n=read(repo_fd, buffer,3);
	buffer[n]='\0';
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
		if(i==3)
		{
			for(j=0;j<4;j++)
			{
				buffer[i]='\0';

			}
			n=read(repo_fd, buffer,3);
			buffer[n]='\0';
			i=0;

		}

	}
	close(repo_fd);

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		pid = fork();
		if (pid < 0)
			error("ERROR on fork");
		if (pid == 0)  {
			close(sockfd);
			dostuff(newsockfd);
			exit(0);
		}
		else close(newsockfd);
	} 
	close(sockfd);
	return 0; 
}

void dostuff (int sock)
{
	time_t tym;
	struct tm * tyme;
	char ttm[80];
	time (&tym);
	tyme = localtime(&tym);
	strftime(ttm,80,"%d-%m-%Y %I:%M:%S",tyme);
	int n,status,repo_fd,i,log_fd;
	char log[256];
	///write menu
	char buffer[256]="Select \n 1.search \n 2.Share \n 3.Exit \n >>";
	n = write(sock,buffer,sizeof(buffer));
	if (n < 0) 
	{
		error("ERROR writing to socket");
		//n = write(sock,'-1',1);  ///test this
	}

	printf("WAITING FOR CLIENT TO RESPOND\n");
	//watng for respose
	bzero(buffer,256);
	status=0;
	while(status==0)
	{
		n = read(sock,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		if(buffer[0]!=0)
			status=1;
	}

	////////client wants to share
	if(buffer[0]=='2')
	{
		char file[256],path[256],ip[256];
		n = write(sock,"\nPATH:",6);
		char query[256]="SHARE REQUEST FROM";

		printf("WAiTING FOR CLIENT TO SHARE FILE\n");
		int k,m,u,a;
		status=0;
		while(status==0)
		{
			n = read(sock,buffer,255);
			if (n < 0) error("ERROR reading from socket");
			if(buffer[0]!=0)
				status=1;
		}

		log_fd=open("./log.txt",O_APPEND | O_RDWR |O_CREAT,0666);
		i=0;
		while(ttm[i]!='\0')
		{
			log[i]=ttm[i];
			i++;
		}
		k=i;
		i=0;
		while(query[i]!='\0')
		{
			log[i+k]=query[i];
			i++;
		}
		log[i+k]='\0';
		n = write(log_fd,log,i+k);
		n = write(log_fd,inet_ntoa(cli_addr.sin_addr),strlen(inet_ntoa(cli_addr.sin_addr)));
		char p='\n';
		n = write(log_fd,&p,1);
		close(log_fd);
		////writing file information shared in repo.txt
		repo_fd=open("./repo.txt",O_APPEND | O_RDWR |O_CREAT,0666);
		for(i=0;i<254;i++)
		{
			if(buffer[i]<32 || buffer[i]>127)
				break;
		}
		buffer[i]='\n';
		i=0;
		k=0;
		m=0;
		bzero(path,256);
		for(i=0;i<254;i++)
		{
			if(buffer[i]<32 || buffer[i]>127)
				break;
			file[k]=buffer[i];
			k++;
			if(buffer[i]=='/')
			{
				for(u=0;u<k;u++)
				{

					path[m+u]=file[u];
				}
				bzero(file,256);
				m=m+k;
				k=0;
			}
		}
		file[k]='#';
		path[m]='#';
		n = write(repo_fd,file,k+1);
		if (n < 0) error("ERROR writing to repo.txt");
		n = write(repo_fd,path,m+1);
		if (n < 0) error("ERROR writing to repo.txt");
		a=cli_addr.sin_addr.s_addr&0xFF;
		ip[0]=a/100+'0';
		a=a%100;
		ip[1]=a/10 + '0';
		a=a%10;
		ip[2]=a + '0';
		ip[3]='.';
		a=(cli_addr.sin_addr.s_addr&0xFF00)>>8;
		ip[4]=a/100+'0';
		a=a%100;
		ip[5]=a/10 + '0';
		a=a%10;
		ip[6]=a + '0';
		ip[7]='.';
		a=(cli_addr.sin_addr.s_addr&0xFF0000)>>16;
		ip[8]=a/100+'0';
		a=a%100;
		ip[9]=a/10 + '0';
		a=a%10;
		ip[10]=a + '0';
		ip[11]='.';
		a=(cli_addr.sin_addr.s_addr&0xFF000000)>>24;
		ip[12]=a/100+'0';
		a=a%100;
		ip[13]=a/10 + '0';
		a=a%10;
		ip[14]=a + '0';
		ip[15]='\n';
		for(i=0;i<254;i++)
		{
			if(ip[i]<32 || ip[i]>127)
				break;
		}
		ip[i]='\n';
		n = write(repo_fd,ip,16);
		if (n < 0) error("ERROR writing to repo.txt");

		close(repo_fd);
		sleep(5);
		readrepo();


		///final response
		n = write(sock,"FILE SHARED SUCCESSFULLY",18);
		if (n < 0) error("ERROR writing to socket");
	}


	////////client wants to search
	else if(buffer[0]=='1')
	{
		char file[256],path[256],ip[256],o;
		n = write(sock,"NAME OF THE FILE TO BE SEARCHED",32);
		char query[256]="SEARCH REQUEST FROM";
		printf("WAITING FOR ClIENT To GIVE NAME\n");
		int k,m,u,a;
		log_fd=open("./log.txt",O_APPEND | O_RDWR |O_CREAT,0666);
		i=0;
		while(ttm[i]!='\0')
		{
			log[i]=ttm[i];
			i++;
		}
		k=i;
		i=0;
		while(query[i]!='\0')
		{
			log[i+k]=query[i];
			i++;
		}
		log[i+k]='\0';
		n = write(log_fd,log,i+k);
		n = write(log_fd,inet_ntoa(cli_addr.sin_addr),strlen(inet_ntoa(cli_addr.sin_addr)));
		char p='\n';
		n = write(log_fd,&p,1);
		close(log_fd);
		status=0;
		while(status==0)
		{
			n = read(sock,buffer,255);
			if (n < 0) error("ERROR reading from socket");
			if(buffer[0]!=0)
				status=1;
		}

		string str(buffer);


		////giving mirrors to client

		str.erase(str.find_last_not_of(" \n\r\t")+1);
		bzero(buffer,256);
		k=0;
		o='1';
		if(map1.find(str)!=map1.end())
		{
			for (std::vector<pair<string,string> >::iterator it = (map1.find(str)->second).begin() ; it != (map1.find(str)->second).end(); ++it)		
			{
				i=0;
				while((str)[i]!='\0')
				{
					buffer[k]=str[i];
					k++;
					i++;
				}
				buffer[k]='#';
				k++;
				int j=0;
				while((it->first)[j]!='\0')
				{
					buffer[k]=it->first[j];
					k++;
					j++;
				}
				buffer[k]='#';
				k++;
				j=0;
				while((it->second)[j]!='\0')
				{
					buffer[k]=it->second[j];
					k++;
					j++;
				}
				buffer[k]='\n';
				k++;
			}
			buffer[k]='\0';
			n= write(sock,buffer,strlen(buffer));
			if (n < 0) error("ERROR writing to socket");
		}
		else
		{
			n = write(sock,"FILE NOT PRESENT",18);
			if (n < 0) error("ERROR writing to socket");
		}
	}
	else if(buffer[0]=='3')
	{
		buffer[0]=-1;
		n = write(sock,buffer,strlen(buffer));
		printf("CLIENT EXiTED SUCCESSFULLY");
		//if (n < 0) error("ERROR writing to socket");
	}
}


void readrepo()
{
	char file[256],path[256],ip[256];
	int k,m,u,j;

	int repo_fd,i,hshct;
	repo_fd=open("./repo.txt",O_RDONLY);
	char buffer[4];


	ssize_t n;
	n=read(repo_fd, buffer,3);
	buffer[n]='\0';
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
		if(i==3)
		{
			for(j=0;j<4;j++)
			{
				buffer[i]='\0';

			}
			n=read(repo_fd, buffer,3);
			buffer[n]='\0';
			i=0;

		}

	}
	close(repo_fd);
}
