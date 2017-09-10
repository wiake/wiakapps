/* **************************************************************************
 Program: wiakf (an IPC, non-interactive, shell)  Version: 2.3.0         
 Creation date (YMD): 2008/01/22
 Revision date (YMD): 2008/02/10
 (C) Copyright 2008 William McEwan (wiakapps A_t wiak D_o_t org)
************************************************************************** */

// wiakf
// is free software in the sense that you can redistribute it and/or
// modify it under the terms of the GNU General Public License as 
// published by the Free Software Foundation, either version 3 of the License, 
// or (at your option) any later version. 
// Refer to http://www.fsf.org/ for further license details.

// Revision history:
// 2.1.0  Created tiny wiak family (twiak). Original released version.
//        Its code is ported from the full version of wiak.
// 2.2.0  Altered --length handling for various --send terminating options.
// 2.3.0  Removed --up logic and simplified channel name scheme etc.
// -------------------------------------------------------------------------

/* Additional Notes:
 (this version "wiakf" is specific for fifo [i.e. named pipe] handling only).
 WIAK is an extremely lightweight, non-interactive shell optimised for
 InterProcessCommunications. It is written in C, using standard libraries, 
 for portability, high speed and resource usage efficiency. It does not 
 require bash or any other underlying shell.
 Unlike most normal shells, wiak only runs for a fraction of a second. As soon
 as it has performed its assigned task it immediately closes, releasing all
 system resources it was using. It is thus a very efficient mechanism.
 WIAK was originally designed to provide a simpler event-driven mechanism for
 the likes of gtkdialog, but it is not restricted to that purpose at all. 
 Refer to http://wiak.org/ for more details and/or examine the code of the 
 available exemplar "wiakapps" to see how wiak is currently used.
 -----------------------------------------------------------------*/

#include "wiakappswiak.h"

void func_optionslong(int argc, char *argv[], int *ptr_fork_result, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st);

void func_wiak_fifo(int argc, char **argv, int *ptr_fork_result, struct wargv *ptr_wargv_st, int *ptr_server_fifo_fd, int *ptr_server_fifo_fd2, struct mbox_all *ptr_msg_st);

void func_wiak_msgqv(int argc, char **argv, int *ptr_fork_result, struct wargv *ptr_wargv_st, int *ptr_server_fifo_fd,  int *ptr_server_fifo_fd2, struct mbox_all *ptr_msg_st);

void func_up_fork_server(char **argv, int *ptr_fork_result, struct wargv *ptr_wargv_st);

void func_send_fifo(int *ptr_server_fifo_fd2, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st);

void func_receive_fifo(int *ptr_server_fifo_fd, int *ptr_server_fifo_fd2, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st);

void func_send_msgqv(int *ptr_msgid, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st);

void func_receive_msgqv(int *ptr_msgid, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st);


int main(int argc, char **argv)
{
    int fork_result = 0, *ptr_fork_result = &fork_result;
    int server_fifo_fd, *ptr_server_fifo_fd = &server_fifo_fd;
    int server_fifo_fd2, *ptr_server_fifo_fd2 = &server_fifo_fd2;
    struct wargv *ptr_wargv_st = &wargv_st;

    struct mbox_all msg_st = {};
    struct mbox_all *ptr_msg_st = &msg_st;

    // if WIAKTMP environment variable is set use its value as the default path 
    // for the storing the wfifo etc., or use the pre-defined default WIAKFIFOPATH.
    // WIAK also has a commandline argument --wiaktmp <arg> for altering this behaviour.
    getenv_ptr = getenv("WIAKTMP");
    if (getenv_ptr)
        strcpy(wiak_tmp_path, getenv_ptr);
    else
        strcpy(wiak_tmp_path, WIAKFIFOPATH);

    func_optionslong(argc, argv, ptr_fork_result, ptr_wargv_st, ptr_msg_st);


    // make directory for wiak fifo(s)
    mkdir(wiak_tmp_path, FIFOPATHPERMS);

    // IPC mode to use
    // named pipes
    func_wiak_fifo(argc, argv, ptr_fork_result, ptr_wargv_st, ptr_server_fifo_fd, ptr_server_fifo_fd2, ptr_msg_st);

    close(server_fifo_fd); // close the fifo for writing but don't remove it
    exit(fork_result);     // fork_result contains either the process id of any [program_name]
                           // started up by wiak (i.e. if wiak is supplied with a non-optional
                           // cmdline argument wiak attempts to execute the argument name)
                           // or the value 0
} /* end_of_main */

void func_optionslong(int argc, char *argv[], int *ptr_fork_result, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st)
{
    char fgets_string[BUF0256_SIZE];
    FILE *fpin;
    int opt;
    struct option long_options[] = {
    {"init", no_argument, NULL, 'i'},          // To tell wiak to create named pipe (i.e. mkfifo)
    {"quit", no_argument, NULL, 'q'},          // To tell wiak to clean up msg box(es) (e.g. rm fifos)
    {"send", required_argument, NULL, 's'},    // arg is the data to send to the message box
    {"receive", required_argument, NULL, 'r'}, // arg is the data to receive from the message box
    {"mode", required_argument, NULL, 'm'},    // Required mode: 
                                               // Currently, fifo (default) or msgqv (Sys V style).
    {"channel", required_argument, NULL, 'c'}, // msg box name, alphanumeric (has a default value).
    {"zid", required_argument, NULL, 'z'},     // msg box name, numeric (has a default value).
                                               // if none of above, will use default fifo name: wfifo
    {"file", required_argument, NULL, 'f'},    // RESERVED (provided as cmdline arg to backend server)
    {"dir", required_argument, NULL, 'd'},     // RESERVED (provided as cmdline arg to backend server)
    {"wiaktmp", required_argument, NULL, 'w'}, // Location of wfifo.
    {"type", required_argument, NULL, 't'},    // type of data to send.
                                               // Default is type CHARSTRDATA as defined in wiakappswiak.h
    {"length", required_argument, NULL, 'l'},  // string length to send; not including terminating \0
    {"perms", required_argument, NULL, 'p'},   // Message box permissions (e.g. for msg queues)
                                               // default is MBOXPERMS as defined in wiakappswiak.h
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {NULL,0,NULL,0}};

     while((opt = getopt_long(argc, argv, "+iqs:r:m:c:z:f:d:w:t:l:p:hv", long_options, NULL)) != -1) 
     {
        switch(opt) 
        {
          case 'i':
        	ptr_wargv_st->wiak_init = TRUE;
            break;
          case 'q':
        	ptr_wargv_st->wiak_quit = TRUE;
            break;
          case 's':
        	ptr_wargv_st->wiak_send = TRUE;
            strcpy(ptr_msg_st->msgdata.msg_char_str.c_data, optarg);
            break;
          case 'r':
        	strcpy(ptr_wargv_st->wiak_receive, optarg);
            break;
          case 'm':
            strcpy(ptr_wargv_st->wiak_mode, optarg);
            break;
          case 'c':
        	strcpy(ptr_wargv_st->wiak_channel, optarg);
            break;
          case 'z':
        	ptr_wargv_st->wiak_zid = atoi(optarg);
            break;
          case 'f':
        	strcpy(ptr_wargv_st->wiak_file, optarg);
	        break;
          case 'd':
        	strcpy(ptr_wargv_st->wiak_dir, optarg);
	        break;
          case 'w':
        	sprintf(wiak_tmp_path, "%s%s", optarg, "/");
	        break;
          case 't':
        	ptr_wargv_st->wiak_type = atoi(optarg);
            break;
          case 'l':
        	ptr_wargv_st->wiak_length = atoi(optarg);
            break;
          case 'p':
        	ptr_wargv_st->wiak_perms = (int)strtol(optarg, NULL, 8);
            break;
          case 'h':
            printf("Usage: \n\n");
			if ((fpin = fopen("/usr/local/wiakapps/wiak/wiakusage.txt","r")) == NULL)
			{
				printf("Error: Can't find file %s\n", "/usr/local/wiakapps/wiak/wiakusage.txt");
				exit(1);
			}			
			while (fgets(fgets_string, sizeof fgets_string, fpin) != NULL)
	  		{
	  			fprintf(stderr, "%s", fgets_string);
	  		}
            exit(0);
            break;
          case 'v':
            printf("wiak version 2.3.0\n");
            exit(0);
            break;
          case ':':
            printf("option needs a value\n");
            exit(0);
            break;
          case '?':
            printf("unknown option: %c\n", optopt);
            printf("Try `wiak --help' for usage information.\n");
            exit(0);
            break;
        }
    }

    // process wiak_length to determine end chars to --send 
    switch(ptr_wargv_st->wiak_length)
    {
      case 0: // zero extra terminating characters appended
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s", ptr_msg_st->msgdata.msg_char_str.c_data);
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data);
        break;
      case 1: // ends with '\n' but no terminating '\0'; this is like default bash echo I think
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\n');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data);
        break;
      case 2: // ends with '\n' followed by a terminating '\0';
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\n', '\0');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data) + 1;
        break;
      case 3: // no '\n' but has terminating '\0';
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\0');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data) + 1;
        break;
      case 4: // ends with '\t' but no '\n' or terminating '\0';
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\t');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data);
        break;
      case 5: // no '\n', but ends with '\t' followed by a terminating '\0';
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\t', '\0');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data) + 1;
        break;
      case 6: // ends with '\t' followed by a '\n' but no terminating '\0';
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\t', '\n');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data) + 1;
        break;
      case 7:  // ends with '\t' followed by a '\n' and then a terminating '\0';
        sprintf(ptr_msg_st->msgdata.msg_char_str.c_data, "%s%c%c%c", ptr_msg_st->msgdata.msg_char_str.c_data, '\t', '\n', '\0');
        ptr_wargv_st->wiak_length = strlen(ptr_msg_st->msgdata.msg_char_str.c_data) + 1;
        break;
    }

    if (optind < argc) // only do this if there is a non--option process to also raise
    {
        *ptr_fork_result = fork();  // fork wiak so ready to overlay with new process
                                    // The parent fork_result contains the child process pid
        if (*ptr_fork_result == 0)  // The child process fork_result contains 0
        {                           // in child
                                    // this call wiak's (i.e. wakes) an optional process
                                    // it receives only those args following its name on the cmdline
          (void) execvp(argv[optind], &argv[optind]);
          exit(EXIT_FAILURE);
        }
    }
}

void func_wiak_fifo(int argc, char **argv, int *ptr_fork_result, struct wargv *ptr_wargv_st, int *ptr_server_fifo_fd,  int *ptr_server_fifo_fd2, struct mbox_all *ptr_msg_st)
{
    // First need to determine the wfifo name before opening it for writing
    // Note that if a backend program was started *ptr_fork_result contains its pid
    if (ptr_wargv_st->wiak_zid == -1234567890) // special case: if --zid value -1234567890
    {
        if (ptr_wargv_st->wiak_channel[0] == '\0')
        {  // use fifo name = backend server process_id
          (void)sprintf(fifo_name, "%s%d", wiak_tmp_path, *ptr_fork_result);
        }
        else
        { // use fifo name = channel value + backend server process_id
          (void)sprintf(fifo_name, "%s%s%d", wiak_tmp_path, ptr_wargv_st->wiak_channel, *ptr_fork_result);
        } 
    }
    else if ((ptr_wargv_st->wiak_channel[0] != '\0') && (ptr_wargv_st->wiak_zid == 0))
           // use fifo name = channel value
           (void)sprintf(fifo_name, "%s%s", wiak_tmp_path, ptr_wargv_st->wiak_channel);
    else if ((ptr_wargv_st->wiak_channel[0] == '\0') && (ptr_wargv_st->wiak_zid == 0))
           // use fifo name = default value = wiakappwiak.h FIFO_DFLT_NAME
           (void)sprintf(fifo_name, "%s%s", wiak_tmp_path, FIFO_DFLT_NAME);
    else if ((ptr_wargv_st->wiak_channel[0] != '\0') && (ptr_wargv_st->wiak_zid != 0))
           // use fifo name = channel value + zid value
           (void)sprintf(fifo_name, "%s%s%d", wiak_tmp_path, ptr_wargv_st->wiak_channel, ptr_wargv_st->wiak_zid);
    else if ((ptr_wargv_st->wiak_channel[0] == '\0') && (ptr_wargv_st->wiak_zid != 0))
    {   // use fifo name = zid value
        (void)sprintf(fifo_name, "%s%d", wiak_tmp_path, ptr_wargv_st->wiak_zid);
    }

    if (ptr_wargv_st->wiak_send == TRUE)
        func_send_fifo(ptr_server_fifo_fd2, ptr_wargv_st, ptr_msg_st);
		
    if (ptr_wargv_st->wiak_receive[0] != '\0')
        func_receive_fifo(ptr_server_fifo_fd, ptr_server_fifo_fd2, ptr_wargv_st, ptr_msg_st);	

    if (ptr_wargv_st->wiak_quit == TRUE)
    {
        close(*ptr_server_fifo_fd);
        close(*ptr_server_fifo_fd2);
        unlink(fifo_name);	
    }
} // end func_wiak_fifo


void func_send_fifo(int *ptr_server_fifo_fd2, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st)
{
   	if ((*ptr_server_fifo_fd2 = open(fifo_name, O_WRONLY)) == -1) 
   	{
       	fprintf(stderr, "Error: failed to open zfifo %s for writing\n", fifo_name);
       	exit(EXIT_FAILURE);
   	}
		
    if (ptr_wargv_st->wiak_type < CHARDATA)
    {   // This is the default char_string data case
        write(*ptr_server_fifo_fd2, ptr_msg_st->msgdata.msg_char_str.c_data, (ptr_wargv_st->wiak_length < 0) ? -(ptr_wargv_st->wiak_length) : ptr_wargv_st->wiak_length);
    }
    else // assuming must be STRUCTDATA for the moment
    {
        write(*ptr_server_fifo_fd2, &(ptr_msg_st->msgdata), sizeof(ptr_msg_st->msgdata));
    }
} /* end_of_func_send_fifo */


void func_receive_fifo(int *ptr_server_fifo_fd, int *ptr_server_fifo_fd2, struct wargv *ptr_wargv_st, struct mbox_all *ptr_msg_st)
{
    if ((*ptr_server_fifo_fd = open(fifo_name, O_RDONLY)) == -1) 
   	{
       	fprintf(stderr, "Error: failed to open zfifo %s for reading\n", fifo_name);
       	exit(EXIT_FAILURE);
   	}
   	if ((*ptr_server_fifo_fd2 = open(fifo_name, O_WRONLY)) == -1) 
   	{
       	fprintf(stderr, "Error: failed to open zfifo %s for writing\n", fifo_name);
       	exit(EXIT_FAILURE);
   	}
		
    if (ptr_wargv_st->wiak_type < CHARDATA)
    {   // This is the default char_string data case
        read(*ptr_server_fifo_fd, ptr_msg_st->msgdata.msg_char_str.c_data, (ptr_wargv_st->wiak_length < 0) ? -(ptr_wargv_st->wiak_length) : sizeof(ptr_msg_st->msgdata.msg_char_str.c_data));
        write(1, ptr_msg_st->msgdata.msg_char_str.c_data, strlen(ptr_msg_st->msgdata.msg_char_str.c_data));
    }
    else // assuming must be STRUCTDATA for the moment
    {
        read(*ptr_server_fifo_fd, &(ptr_msg_st->msgdata), (ptr_wargv_st->wiak_length == 0) ? sizeof(ptr_msg_st->msgdata) : ptr_wargv_st->wiak_length);

        write(1, ptr_msg_st->msgdata.msg_char_str.c_data, strlen(ptr_msg_st->msgdata.msg_char_str.c_data));
    }
} /* end_of_func_receive_fifo */


