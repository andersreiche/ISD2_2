#ifndef ISD2_2_H
#define ISD2_2_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>
#include <stdlib.h>
#include <fstream>

#define AIN0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

using namespace std;


char str[100];
int listen_fd;
int comm_fd;
int n = 0;
FILE *fp = NULL;
pid_t process_id = 0;
pid_t sid = 0;
float temp = 0; //Placeholder temperature reading

void setup_ADC(void) {
    system("echo BB-ADC > /sys/devices/platform/bone_capemgr/slots");
}

float convert_temp(int temp) {
    float milivolts = (temp / 4096) * 1800;
    return (milivolts-500) / 10;
} 

float get_temp(void) {
    string line;
    ifstream myfile (AIN0);
    if (myfile.is_open()) {
        getline (myfile,line);
    }
    float degC = convert_temp(stoi(line));
    return degC;
}

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

void timer_handler (int signo) {
    if (signo == SIGALRM) {
        to_syslog("Got timer interrupt");
        temp = get_temp();
    }
}

void get_input(void) {
	bzero(str, 100); //sets str to ZERO
	n = read(comm_fd, str, 100); //reads from the client to str and puts number of signs in to n
	string stringstr = str;
        to_syslog("#DEBUG#" + stringstr + "#DEBUG#");
        std::size_t found = stringstr.find("GET TEMP");
	if (found!=std::string::npos) {
            to_syslog("GET TEMP was recieved");
            stringstr = "Temperature is " + tostr(temp) + " degC.\n";
            strncpy(str, stringstr.c_str(), sizeof(str));
            write(comm_fd, str, sizeof(str)); //writes back to the client
	}
}
#endif	// ISD2_2_H

