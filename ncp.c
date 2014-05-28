#define TL_HELP_MESSAGE		\
"\nncp (v1.3)\
\n==================\
\n To Receive a File:\
\n ------------------\
\n %s [--output <\"Name\">]\
\n\
\n  --output: Override the default output file name (ncp.out)\
\n\
\n To Send a File - Simple Syntax:\
\n -------------------------------\
\n %s <File> <ip>[:<port>]\
\n\
\n  The simple syntax can be thought of the \"traditional\" source-destination\
\n  format that can be used in place of flags/long syntax.\
\n\
\n To Send a File - Long Syntax:\
\n -----------------------------\
\n %s --input <\"Name\"> --ip ###.###.###.### [--port #####]\
\n\
\n  --input:  File to open for sending over the network\
\n  --ip:     This is the IP of the Listening Server\
\n  --port:     This is the IP of the Listening Server\
\n\
\n Other Optional Flags:\
\n ---------------------\
\n  [-v or --verbose]: Output verbose messages\
\n  [--help]: prints this help message\
\n\
\n Release Notes:\
\n --------------\
\n  By default this program will run in listening mode, and will wait for a\
\n  client to send a file. The default output file is ncp.out, and can be \
\n  overriden by the --output argument.\
\n\
\n  No attempts at security nor reliability have been made with respect to the\
\n  development of this software.\
\n\n                                                  ActuallyFro - 2014\
\n\n",TL_ProgName,TL_ProgName,TL_ProgName

#include "TL_0_03.h"

int main (int argc, char **argv){
	int	Verbose=0;
	int	MODE=0;
	char	IP_Dest[50]="127.0.0.1";
	char	IP_Port_Str[10];
	int		IP_Port=1337;
	char	OutputFile[500]="ncp.out";
	//Parse Args Vars
	char	*IP_Input_Dest = NULL;
	char	*IP_Input_Port = NULL;
	char	*File_Input = NULL;
	char	*File_Output = NULL;

	TL_PARSEARGS_INSTALL();
	TL_PARSEARGS_START(argc, argv,TL_ENFORCEPARSING_ON)
		TL_PARSEARGS_ADD_STR("--input",File_Input)
		TL_PARSEARGS_ADD_STR("--output",File_Output)
		TL_PARSEARGS_ADD_STR("--ip",IP_Input_Dest)
		TL_PARSEARGS_ADD_STR("--port",IP_Input_Port)
		TL_PARSEARGS_ADD_STR("-p",IP_Input_Port)
		TL_PARSEARGS_ADD_FLAG("-v", Verbose, 1)
		TL_PARSEARGS_ADD_FLAG("--verbose", Verbose, 1)
		TL_PARSEARGS_ENFORCE_EXCEPTION_ARGC_EQUALS_N(3)
	TL_PARSEARGS_STOP

	TL_IPv4_INIT();
		TL_IPv4_STR_CHECKING_VARS();
		TL_IPv4_SOCKET_TCP_NCP_INIT();

	TL_IPv4_SOCKET_INIT(TCPClientEx);
	TL_IPv4_SOCKET_INIT(TCPServerListener);
	TL_IPv4_SOCKET_INIT(TCPClientResponder);	

	if(Verbose==1){TL_DEBUGGING_ENABLE;}
	
	if(File_Input!=NULL || TL_Enforce_Exception_Occured==1){//Input and IP are tied together
		MODE=1; //If file input then ensure "sending" mode is set
		
		if(TL_Enforce_Exception_Occured==1){ //Simple Syntax -- IP; above parsing causes argv[0] to be removed, and left shifts all the "indexing"
			File_Input = argv[0]; //File input
			TL_IPv4_PARSE_IP_AND_PORT(argv[1],IP_Dest,IP_Port_Str);
			if(TL_IPv4_Colon_found==1){
				TL_IPv4_PORT_CHECK_STR(IP_Port_Str,TL_ENFORCEPARSING_ON);
				IP_Port = atoi(IP_Port_Str);
			}
			TL_IPv4_IP_CHECK_STR(IP_Dest,TL_ENFORCEPARSING_ON);
		}
		else{ //Long Syntax -- IP
			//TL_IPv4_PARSE_IP_AND_PORT(IP_Input_Dest,IP_Dest,IP_Port_Str);	//If a port is found in this IP it's black holed to IP_Port_Str
			if(IP_Input_Dest!=NULL){
				TL_IPv4_IP_CHECK_STR(IP_Input_Dest,TL_ENFORCEPARSING_ON);
				strcpy(IP_Dest,IP_Input_Dest);
			}
			else{
				printf("\n[WARNING] No IP given! Using default: %s",IP_Dest);
			}
		}
	}
	
	if(IP_Input_Port!=NULL && TL_Enforce_Exception_Occured==0){ //Long Syntax -- Ports
		TL_IPv4_PORT_CHECK_STR(IP_Input_Port,TL_ENFORCEPARSING_ON);  //The port is assumed to come from one of the port flags
		IP_Port = atoi(IP_Input_Port);
	}

	if(File_Output!=NULL && strlen(File_Output)<500) {
        	strcpy(OutputFile,File_Output);
	}
	
	if(MODE==1){
		TL_DEBUGF(stdout,"\nVerbose Output: Sending Mode");
		TL_DEBUGF(stdout,"\n============================");

		TL_IPv4_SOCKET_TCP_INIT(TCPClientEx);
		TL_IPv4_SOCKET_SETIP(TCPClientEx,IP_Dest); 
		TL_IPv4_SOCKET_SETPORT(TCPClientEx,IP_Port);
		
		TL_IPv4_SOCKET_TCP_CONNECT(TCPClientEx);

		if(TL_Enforce_Exception_Occured==1){TL_DEBUGGING_ENABLE;}
		TL_DEBUGF(stdout,"\nSending '%s' to remote server on '%s:%i' \n", File_Input, IP_Dest, IP_Port);
		if(TL_Enforce_Exception_Occured==1){TL_DEBUGGING_DISABLE;}
		
		TL_IPv4_SOCKET_TCP_NCP_SEND(TCPClientEx,File_Input);
		
		TL_IPv4_SOCKET_CLOSE(TCPClientEx);	
	}
	else{//Receiver
		TL_DEBUGF(stdout,"\nVerbose Output: Listening Mode");
		TL_DEBUGF(stdout,"\n==============================");
		TL_DEBUGF(stdout,"\nOutput File:\t%s",OutputFile);
		
		TL_IPv4_SOCKET_TCP_INIT(TCPServerListener);
		TL_IPv4_SOCKET_SETPORT(TCPServerListener,IP_Port);

		TL_DEBUGF(stdout,"\nCurrent IP Address: %s",inet_ntoa(TCPServerListener_ip_addr));
		TL_DEBUGF(stdout,"\nCurrent Port Number: %d",ntohs(TCPServerListener_addr.sin_port));
		TL_IPv4_SOCKET_TCP_BIND_and_LISTEN_INIT(TCPServerListener,1);

		TL_IPv4_SOCKET_TCP_ACCEPT(TCPServerListener,TCPClientResponder);
		memcpy(&TCPClientResponder_ip_addr, &TCPClientResponder_addr.sin_addr.s_addr, 4);
		TL_DEBUGF(stdout,"\n\tClient Connected! (%s:%d)",inet_ntoa(TCPClientResponder_ip_addr), ntohs(TCPClientResponder_addr.sin_port));

    	TL_DEBUGF(stdout,"\nReceiving remote executable file\n");
		TL_IPv4_SOCKET_TCP_NCP_RECV(TCPClientResponder,OutputFile);
		
		TL_DEBUGF(stdout,"\n\nClosing down connection!");
		TL_IPv4_SOCKET_CLOSE(TCPServerListener);
		TL_IPv4_SOCKET_CLOSE(TCPClientResponder);
	}
	
	
	TL_IPv4_Cleanup();
	printf("\n");
	
	return EXIT_SUCCESS;
}

