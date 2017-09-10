/* **************************************************************************
 This is the main #include header file for the program: wiak Version: 2.2.0 
 Creation date (YMD): 2007/12/10
 Revision date (YMD): 2008/01/31
 (C) Copyright 2007 William McEwan (wiakapps A_t wiak D_o_t org)
************************************************************************** */
/*
 wiak itself (the IPC subsystem shell) 
 is free software in the sense that you can redistribute it and/or
 modify it under the terms of the GNU General Public License as 
 published by the Free Software Foundation, either version 3 of the License, 
 or (at your option) any later version. 
 Refer to http://www.fsf.org/ for further licence details.

// Revision history:
// 1.0.0  Original released version.
// 1.1.2  Added options --receive and --quit
// 2.0.0  Altered main data structures and added message queue functionality
//        Changed header name to wiakappswiak.h
// 2.1.0  Created tiny wiak family (twiak). Changed msgq option to msgqv.
// 2.2.0  Altered --length handling for various --send terminating options.

 Additional Notes:

 Some parts of this are not used in the currently published version of WIAK
 but are required for compatibility with other wiak.org wiakapp 
 developments. These parts are marked as "RESERVED and subject to change".

 In particular, wiak currently only officially accepts simple character
 string data (via its --send option) for passing on into a named pipe 
 (i.e. fifo). However, internally, wiak is already capable of sending a
 much more complex binary data structure, which takes the form of the 
 C structure msgdata_struct shown below (and the wiak.org program "morfi"
 is capable of reading that type of data from a fifo). Though there is no 
 need to ever use that structure type in any client or server, since simple
 character string data passing will always be supported, there may be
 designs which would benefit from the more complex data type. Having said
 that, the wiak binary struct sending mode is currently only at the
 development stage and the format of the msgdata_struct binary data type
 is likely to change in future wiak releases.
 */

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define WIAKFIFOPATH "/tmp/wiak/"
#define FIFOPATHPERMS 0666
#define MBOXPERMS 0666
#define FIFO_DFLT_NAME "wfifo" // default fifo name
#define MSGQV_DFLT_NAMEKEY 45678
#define WAITFIFO 5

#define BUF65536_SIZE 65536
#define BUF32768_SIZE 32768
#define BUF16384_SIZE 16384
#define BUF8192_SIZE 8192
#define BUF4096_SIZE 4096
#define BUF0256_SIZE 256
#define BUF0128_SIZE 128
#define BUF0064_SIZE 64
#define BUF0032_SIZE 32
#define BUF0016_SIZE 16
#define BUF0008_SIZE 8
#define BUF0004_SIZE 4

#define MAX_FIFOMSG BUF65536_SIZE 
#define TRUE 0
#define FALSE 1

// The following #defines are RESERVED and subject to change:
#define CHARSTRDATA 1     // Range 1 to 2047.
#define CHARDATA 2048     // planned Range 2048 to 4095 RESERVED
#define LONGINTDATA 4096  // planned Range 4096 to 6143 RESERVED
#define DOUBLEDATA 6144   // planned Range 6144 to 8191 RESERVED
#define STRUCTDATA 8192   // Experimental, uses 8192 and up RESERVED

static key_t msgqv_namekey = MSGQV_DFLT_NAMEKEY; // key (like a numeric name) to use for the msgqv
static char fifo_name[PATH_MAX] = {'\0'}; // complete fifo name
static char wiak_tmp_path[PATH_MAX] = {WIAKFIFOPATH}; // default fifo path
char *getenv_ptr;

// currently only using CHARSTRDATA
// Any code for STRUCTDATA messages is purely experimental
// Currently can --send messages of max sizes =
// 65536 bytes for fifo IPC and 8192 for SysV message queue IPC
// The above values are system/platform dependent. Results are from
// a Linux system with kernel version 2.6.21.5

struct mbox_char { // RESERVED and subject to change
    long int msgtype; // msg queue C-char type.
                      // Initialise to CHARDATA
    char c_data;
};

struct mbox_long { // RESERVED and subject to change
    long int msgtype; // msg queue C-char type.
                      // Initialise to LONGINTDATA
    long int c_data;
};

struct mbox_char_str {
    long int msgtype; // msg queue C-char_stringdata type.
                      // Initialise to CHARSTRDATA
    char c_data[MAX_FIFOMSG]; // to/from mbox channel to/from wargv_st->wiak_send buffer
};

struct mbox_double { // RESERVED and subject to change
    long int msgtype; // msg queue C-char type.
                      // Initialise to DOUBLEDATA
    double c_data;
};

struct msgdata_struct { // used in fifo and SysV msgqueue mode
    struct mbox_char msg_char; // RESERVED and subject to change
    struct mbox_long msg_long; // RESERVED and subject to change
    struct mbox_char_str msg_char_str; // used in fifo and SysV msgqueue mode
    struct mbox_double msg_double; // RESERVED and subject to change
        
    pid_t pid_t_array[BUF0004_SIZE]; // RESERVED and subject to change
};

struct mbox_all {
    long int msgtype; // msg queue C_struct data type.
                      // Initialise to STRUCTDATA
    struct msgdata_struct msgdata;
};

struct wargv {
	char wiak_up[BUF0256_SIZE];   // program <path>/name of wiakapp backend server to raise
	char wiak_mode[BUF0016_SIZE]; // default is fifo (alternatives: msgqv, ... others coming)
	char wiak_send;	    // set to FALSE (=1) for no --send option selected (i.e. nothing to send)
                        // and TRUE (=0) for --send option selected (i.e. something to send)
	char wiak_receive[BUF0256_SIZE];  // device on which wiak should receive mbox message
	                                  // currently, user should specify stdout
	char wiak_channel[BUF0256_SIZE];  // alphanumeric name for the mbox channel (optional)
                                      // else fifo uses default ch name [+ any zid value]
	pid_t wiak_zid;  // numeric id for identifying the mbox channel (optional)
	char wiak_file[BUF0256_SIZE]; 	// RESERVED and subject to change
	char wiak_dir[BUF0256_SIZE];    // RESERVED and subject to change
	long int wiak_type; // type/priority of messages to send (used in mess queue mode)
                        // default is CHARSTRDATA = char_string_c_data.
                        // Can use for prioritising SysV message queue data.
                        // struct_c_data messages (STRUCTDATA) are currently experimental
                        // and subject to changes in format
	long int wiak_length; // negative values directly indicate string length to --send
                          // positive values indicate terminating chars to --send
                          // 0 = don't append anything; 1 = '\n' only; 2 = '\n' then a '\0'; 
                          // 3 = '\0' only (i.e. a simple C char string);
                          // 4 = '\t' only; 5 = '\t' then a '\0'; 
                          //  6 = '\t', then a '\n'; 7 = '\t', then a '\n' and then a '\0'; 
                          // default is 1, which results in the same behaviour as the default  
                          // (no options used) bash echo command
	int wiak_perms;       // message box (octal rwx) permissions (used in mess queue mode)
	char wiak_init;	      // set to FALSE (=1) for don't make new channel (e.g. fifo)
                          // and TRUE (=0) for make new channel (e.g. create new fifo)
	char wiak_quit;	    // set to FALSE (=1) for don't clean up
                        // and TRUE (=0) for clean up e.g. remove fifo(s) or SysV message queue(s)
}wargv_st =
{ //defaults
    "",
    "",
    FALSE,
    "",
    "",
    0,
    "wiakrecord.conf",
    "/usr/local/wiakapps/morfi/",
    CHARSTRDATA,
    1,
    MBOXPERMS,
    FALSE,
    FALSE
};
