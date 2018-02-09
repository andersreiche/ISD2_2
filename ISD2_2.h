#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

float get_temp(void);

char str[100];
int listen_fd;
int comm_fd;
int n = 0;
FILE *fp = NULL;
pid_t process_id = 0;
pid_t sid = 0;

float temp = 22.5; // Placeholder temperature reading

template <typename T> string tostr(const T& t) { 
   ostringstream os; 
   os<<t; 
   return os.str(); 
} 

void to_syslog(string str) {
    openlog("Daemonisering ", LOG_PID, LOG_USER);
    syslog(LOG_INFO, str.c_str());
    closelog();
}

void sig_handler(int signo) {
	openlog("Daemonisering ", LOG_PID, LOG_USER);
	if (signo == SIGHUP)
		syslog(LOG_INFO, "SIGHUP recieved, exiting");
	closelog();
	exit(0);
}

void get_input(void) {
	bzero(str, 100); //sets str to ZERO
	n = read(comm_fd, str, 100); //reads from the client to str and puts number of signs in to n
	string stringstr = str;
        std::size_t found = stringstr.find("GET TEMP");
	if (found!=std::string::npos) {
            to_syslog("GET TEMP was recieved");
            stringstr = "Temperature is " + tostr(get_temp()) + " degC.";
            strcpy(str, stringstr.c_str());
            write(comm_fd, str, sizeof(str)); //writes back to the client
	}
}

float get_temp(void) {
    // TODO Make this
    return temp;
}