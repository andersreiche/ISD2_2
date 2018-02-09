//============================================================================
// Name        : ISD2_2.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "ISD2_2.h"

void sig_handler(int signo);
void get_input(void);


int main(int argc, char* argv[]) {
	process_id = fork(); // Create child process

	if (process_id < 0) { // Indication of fork() failure
		printf("fork failed!\n");
		exit(1);
	}

	if (process_id > 0) { // PARENT PROCESS. Need to kill it.
		printf("process_id of child process %d \n", process_id);
		exit(0);
	}

	umask(0); //unmask the file mode granting read/write/exe rights

	sid = setsid(); //set new session
	if (sid < 0) {
		exit(1);
	}

	chdir("/"); // Change the current working directory to root.
	// Close stdin. stdout and stderr, daemonizing the process
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
        to_syslog("Started daemonized process");       

	// Calls the signal handler if SIGHUP is recieved
	if (signal(SIGHUP, sig_handler) == SIG_ERR) {
		exit(0);
	}
        
        struct sockaddr_in servaddr;
	listen_fd = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM = TCP socket
	bzero(&servaddr, sizeof(servaddr)); //alternative to memset but only can set ZERO

	/* Initialize socket structure */
	servaddr.sin_family = AF_INET; //Ip V4
	servaddr.sin_addr.s_addr = htons(INADDR_ANY); //accept any of the interfaces
	servaddr.sin_port = htons(1955); //Port number

	if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) { // testing if the bind goes well
		perror("ERROR on binding");
		exit(1);
	}
        
        listen(listen_fd, 10);
	//listening to the specified ip and port and have a waiting list of 10
	comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
        
	while (1) {
		//Dont block context switches, let the process sleep for some time
		sleep(1);
		get_input();
	}
	fclose(fp);
	return (0);
        to_syslog("Daemonized process ended");
}
