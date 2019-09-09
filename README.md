This project has mainly two parts:


1. Repo server

It is a program to create a central repo server which would handle the access to central share repository. Central repository is a simple file which maintains a map of file path and machine ip, from which the file can be downloaded Program listens and responds to following operations

● Search for a file in repo, and return the mirrors

● Add a new share entry in the repo file as requested by a client

● Maintain a separate log file which logs all the incoming and outgoing events at the server. Name the file “repo.log” and place it in the same directory as the executable.

● Following is the logging format. Log all events on a new line.

        ○ <timestamp>: <event­description>

■ E.gs:

■ 28­08­2016 16­30­27: Search request from 10.1.1.2

■ 28­08­2016 16­30­27: Search response send to 10.1.1.2

■ 28­08­2016 16­30­47: Share request from 10.1.1.2

■ 28­08­2016 16­30­47: Share ack sent to 10.1.1.2

Note: Repo server only handles operation on the repo.txt file. Such as add a new entry or
retrieve desired mirrors for a search query. It does not act as a file download server.
Run the repo server program as below, where 19000 is the port on which the server listens.


2. Client

It is a simple client program(which doubles up as a server for other nodes to download file) which will provide a menu driven interface to do the following:

● Search for a file to download

● Share a file on central repository

● Also this same client, should fork a new process to run a file download server parallely. Again you need to design your own wire protocol to understand incoming requests and respond accordingly.

● Maintain a separate log file which logs all the incoming and outgoing events at the server. Name the file “client.log” and place it in the same directory as the executable.

● Following is the logging format. Log all events on a new line.

        ○ <timestamp>: <event­description>

■ E.gs:

■ 28­08­2016 16­30­27: Download request from 10.1.1.2

■ 28­08­2016 16­30­27: File sent to 10.1.1.2

Search feature will allow user to add keywords and that would be queried to central repository via sockets. The result returned would be a list of options(mirrors) from which the file can be downloaded. Once a mirror is selected the file must be downloaded from the
selected mirror.
Share a file, is a simple feature where user gives in a filepath to share. The same is queried to repo server to append a new entry in remote repository. Run the client program as below, where
● Argument 1 : ip of the machine where repo server is running. (localhost or 127.0.0.1 for local machine)

● Argument 2 : port number on which the repo server is hosted
