//Trojan Lib v _____

/*So mingw uses its printf not msvcrt \*/
#define _ISOC99_SOURCE

// TCP and UDP Code is based on the work of Dr. Ken Christenson, whom only attributes, but does not provide a specific license.
// Attribution/Contact info from source
//=  Author: Ken Christensen                                                  =
//=          University of South Florida                                      =
//=          WWW: http://www.csee.usf.edu/~christen                           =
//=          Email: christen@csee.usf.edu                                     =
// Code pulled from: http://www.csee.usf.edu/~christen/tools/toolpage.html#tcpip

#define WINDOWS defined(_WIN32) || defined (_WIN64)
#define NOTWINDOWS defined (__linux__) || defined (__APPLE__) || defined (__ANDROID__) || defined(__FreeBSD__)

//http://sourceforge.net/p/predef/wiki/Architectures/
//http://en.wikipedia.org/wiki/C_preprocessor
#define OS_BITS64 defined(__itanium__) || defined(__ia64__) || defined(__x86_64__) || defined(__amd64__) || defined(__aarch64__) || defined(__LP64__) || defined (__LLP64__) || defined (_WIN64)
#define OS_BITS32 defined(__arm__) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(_X86_) || defined(__IA32__) || defined(_WIN32)

#if OS_BITS32
	#define _FILE_OFFSET_BITS 64
#endif
/*Randoms from the Inet:
	#define define _FILE_OFFSET_BITS 64
	#define _FILE_OFFSETBITS 64
	#define _FILE_OFFSET_BITS 64 
	#define _LARGEFILE64_SOURCE 1 
	#define _LARGEFILE_SOURCE 1 

	==> OPEN() WORKS on *Nix/BSD. Windows simply needs the O_BINARY flag set for open to work appropriately!
*/

/*
#if WINDOWS
	#define TL_FILE_OPEN_READ_FLAGS O_RDONLY | O_BINARY 
	#define TL_FILE_OPEN_WRITE_NEWFILE_FLAGS O_WRONLY | O_CREAT | O_BINARY
	#define TL_FILE_OPEN_WRITE_APPENDFILE_FLAGS O_WRONLY | O_APPEND | O_BINARY
#else
	#define TL_FILE_OPEN_READ_FLAGS O_RDONLY
	#define TL_FILE_OPEN_WRITE_NEWFILE_FLAGS O_WRONLY | O_CREAT
	#define TL_FILE_OPEN_WRITE_APPENDFILE_FLAGS O_WRONLY | O_APPEND
#endif
*/
	#include <inttypes.h>
	#include <limits.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <time.h>
	#include <sys/types.h>	// Needed for system defined identifiers.
	#include <sys/stat.h>	
	//#include <sys/timeb.h>	// Needed for ftime() and timeb structure
	
#if WINDOWS
	#include <windows.h>
	#include <conio.h>
	#define TL_ENTER	13
	#define TL_BACKSPACE	8
	#define TL_ESCAPE	27
#elif NOTWINDOWS
	#include <termios.h>
	#include <unistd.h>	// for read()

	#include <netinet/in.h>	// Needed for internet address structure.
	#include <sys/socket.h>	// Needed for socket(), bind(), etc...
	#include <arpa/inet.h>	// Needed for inet_ntoa()
	#include <fcntl.h>	// Needed for sockets stuff
	#include <netdb.h>	// Needed for sockets stuff

	#define KBHIT_H //Is this needed?
	#define TL_ENTER		10
	#define TL_BACKSPACE	127
	#define TL_ESCAPE		27
	#define TL_DELETE		126
#endif


// Used for "while(!kbhit)" styled loops
#define TL_SPACE		32
#define TL_TAB			9

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

/*
#ifndef O_BINARY 
	#define O_BINARY O_EXCL 
#endif
*/

#ifndef NULL
	#define NULL ((void*)0)
#endif

#if WINDOWS
	#define	TL_KBHIT_LOOP_INSTALL(NAME)	int TL_FOREVER_LOOP_##NAME = 1; int TL_GET_KEY_##NAME
	#define TL_KBHIT_LOOP_STOP	}
#elif NOTWINDOWS
	#define	TL_KBHIT_LOOP_INSTALL(NAME)	int TL_FOREVER_LOOP_##NAME = 1; int TL_GET_KEY_##NAME; init_keyboard();
	#define TL_KBHIT_LOOP_STOP	}; close_keyboard();
#endif

#define TL_KBHIT_LOOP_START(NAME)	while(TL_FOREVER_LOOP_##NAME==1){

#define TL_CHECK_KEY_SWITCH_START(NAME)     \
		if ( kbhit()){                      \
			TL_GET_KEY_##NAME = getch();	\
			switch(TL_GET_KEY_##NAME){

#define	TL_CHECK_KEY_SWITCH_STOP	\
			}                       \
		}

#define TL_KBHIT_CASE_QUIT(NAME)	TL_FOREVER_LOOP_##NAME=0

#if NOTWINDOWS
	static struct termios initial_settings, new_settings;
	static int peek_character = -1;

	void init_keyboard(){

		tcgetattr(0,&initial_settings);
		new_settings = initial_settings;
		new_settings.c_lflag &= ~ICANON;
		new_settings.c_lflag &= ~ECHO;
		new_settings.c_lflag &= ~ISIG;
		new_settings.c_cc[VMIN] = 1;
		new_settings.c_cc[VTIME] = 0;
		tcsetattr(0, TCSANOW, &new_settings);
	}

	void close_keyboard(){
		tcsetattr(0, TCSANOW, &initial_settings);
	}

	int kbhit(){
		unsigned char ch;
		int nread;

		if (peek_character != -1)
			return 1;

		new_settings.c_cc[VMIN]=0;
		tcsetattr(0, TCSANOW, &new_settings);
		nread = read(0,&ch,1);
		new_settings.c_cc[VMIN]=1;
		tcsetattr(0, TCSANOW, &new_settings);
		if(nread == 1){
			peek_character = ch;
			return 1;
		}

		return 0;
	}

	int getch(){
		char ch;

		if(peek_character != -1){
			ch = peek_character;
			peek_character = -1;
			return ch;
		}
		read(0,&ch,1);
		return ch;
	}
#endif

int TL_DEBUGGING=0;
#define TL_DEBUGF(...)			\
	if(TL_DEBUGGING==1)		\
		fprintf(__VA_ARGS__)

#define TL_DEBUGGING_ENABLE TL_DEBUGGING=1

#define TL_DEBUGGING_DISABLE TL_DEBUGGING=0

#define DEBUGGING_FILE_FD	stderr

int TL_STDIN_PIPED=0;
#define STDIN_CHECK()										\
if (isatty(fileno(stdin))){									\
	TL_STDIN_PIPED=0;									\
	TL_DEBUGF(stdout,"\n[TL DEBUGGING] This program's stdin WAS NOT a REDIRECT");		\
    }												\
else{												\
	TL_STDIN_PIPED=1;									\
	TL_DEBUGF(stdout,"\n[TL DEBUGGING] This program's stdin WAS a REDIRECT");			\
}

#define TL_2D_Array_Create(TYPE,NAME,ROWS,COLS)							\
	TYPE **NAME;										\
	int rows_##NAME = ROWS;									\
	int cols_##NAME = COLS;									\
	int TL_i_##NAME;									\
	NAME = malloc( ROWS * sizeof(TYPE *));							\
	if(NAME == NULL){									\
		TL_DEBUGF(stderr,"\n[TL_ERROR] out of memory to create Array NAME");		\
		exit;										\
	}											\
	for( TL_i_##NAME = 0; TL_i_##NAME < ROWS; TL_i_##NAME++){				\
		NAME[TL_i_##NAME] = malloc( COLS * sizeof(TYPE));				\
		if( NAME[TL_i_##NAME] == NULL){							\
			TL_DEBUGF(stderr,"\n[TL_ERROR] out of memory to create Array NAME");	\
			exit;									\
		}										\
	}

#define TL_2D_Char_Array_FREE(NAME)								\
	for( TL_i_##NAME = 0; TL_i_##NAME < rows_##NAME ; TL_i_##NAME++){			\
		free( NAME[TL_i_##NAME]);							\
	}											\
	free( NAME )

int TL_Default_Set_Array_i;
int TL_Default_Set_Array_j;

#define TL_2D_Array_Set_Default(NAME,VALUE)										\
	for(TL_Default_Set_Array_i = 0; TL_Default_Set_Array_i < rows_##NAME; TL_Default_Set_Array_i++){		\
		for(TL_Default_Set_Array_j = 0; TL_Default_Set_Array_j < cols_##NAME; TL_Default_Set_Array_j++){	\
			NAME[TL_Default_Set_Array_i][TL_Default_Set_Array_j] = VALUE;					\
		}													\
	}

//NEEDS int argc and char **argv as MAIN arguments
#define TL_ENFORCEPARSING_ON 1
#define TL_ENFORCEPARSING_OFF 0

#define TL_PARSEARGS_INSTALL()	\
	int TL_help=0;				\
	int EnforceParsing;			\
	char TL_ProgName[500];		\
	int TL_Initial_Argc = 0;	\
	int TL_Enforce_Exception_Occured

#if !defined(TL_HELP_MESSAGE)
    #define TL_HELP_MESSAGE "\nUsage: %s [-file <filename>] [-double <arg>] [-int <arg>] [-flag]\n",TL_ProgName
#endif
	
#define TL_PARSEARGS_START(argc,argv,Enforce)		\
	EnforceParsing=Enforce;			\
	TL_Initial_Argc = argc;						\
	TL_Enforce_Exception_Occured=0;\
	strcpy(TL_ProgName,argv[0]);		\
	argc--; argv++;				\
	while(argc > 0){			\
		if(!strcmp(*argv, "--help")){	\
		argv++; argc--; TL_help=1;		\
		}
        
#define TL_PARSEARGS_STOP													\
		else {													\
          fprintf(stderr,TL_HELP_MESSAGE);  \
		  fprintf(stderr, "\nUnrecognized recognized command line argument: %s\n\n", *argv);			\
		  if(EnforceParsing==1){return 0;}									\
		}													\
		argv++;argc--;												\
	} 														\
if(TL_help){   \
    fprintf(stderr,TL_HELP_MESSAGE);    \
	return 1;                                       \
  }

#define TL_PARSEARGS_ADD_INT(x,y)					\
	else if (!strcmp(*argv, x)){			\
		argv++; argc--; y = atoi(*argv);	\
	}

#define TL_PARSEARGS_ADD_FLOAT(x,y)				\
	else if (!strcmp(*argv, x)){			\
		argv++; argc--; y = atof(*argv);	\
		}

#define TL_PARSEARGS_ADD_STR(x,y)				\
	else if (!strcmp(*argv, x)){		\
		argv++; argc--; y = *argv;	\
	}

#define TL_PARSEARGS_ADD_FLAG(x,y,Value)			\
	else if (!strcmp(*argv, x)){		\
		y = Value;			\
	}
#define TL_PARSEARGS_ENFORCE_EXCEPTION_ARGC_EQUALS_N(ExceptionN)			\
	else if (TL_Initial_Argc == ExceptionN){		\
		TL_Enforce_Exception_Occured=1;	break;\
	}
	
#if WINDOWS
	#include <io.h>
	#include <fcntl.h>
	#define STRCMP strcmp
	#define USE_CRLF 1
#elif NOTWINDOWS
	#define STRCMP strcmp
	#define USE_LF 1
#endif

#define TL_B64_OK	0
#define TL_B64_ERR	1

#define TL_B64_EN_UDP_SIZE_IN (234*3)  //MTU SIZES for UDP
#define TL_B64_EN_UDP_SIZE_OUT (234*6) //MTU SIZES for UDP

#define TL_B64_EN_TCP_SIZE_IN (8192*3)
#define TL_B64_EN_TCP_SIZE_OUT (8192*6)

#define TL_B64_DE_SIZE_IN (8192*4)
#define TL_B64_DE_SIZE_OUT (8192*3)

#define	TL_B64_INIT_Global()				\
	static int TL_B64_DECODE_FLAG = FALSE;		\
	static int TL_B64_fUseCRLF = FALSE;		\
	static char *TL_B64_FILE_IN = NULL;		\
	static FILE *TL_B64_FILE_IN_FD = NULL;		\
	static char *TL_B64_FILE_OUT = NULL;		\
	static FILE *TL_B64_FILE_OUT_FD = NULL;		\
	static unsigned int TL_B64_LINE_CHARS = 64

#define TL_B64_INIT()					\
	static const char* base64_lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";	\
    unsigned long nDiv = 0;				\
    unsigned long nRem = 0;				\
    unsigned long nChars = 0;			\
    unsigned int nOut = 0;				\
    size_t nWritten = 0;				\
    size_t nRead;						\
    size_t n;							\
    int r = TL_B64_OK;					\
    unsigned char* TL_B64_encodingPtr;  \
    int z=0;                            \
    unsigned char bits;                 \
    char c = '\0';                      \
    unsigned char data[3];              \
    int TL_B64_DECODE_COUNTER_INPUT;    \
    int TL_B64_DECODE_COUNTER_OUTPUT;   \
    unsigned int nData = 0

#define TL_B64_ENCODE_FROM_FD(FROM,TO,SIZEBUF,FILEPATH)										\
	for(;;){														\
        if( FROM == NULL || TO == NULL ){											\
            r = TL_B64_ERR;													\
            break;														\
        }															\
        for(;;){														\
			nDiv = 0;												\
            nRem = 0;														\
            nChars = 0;														\
            TL_B64_encodingPtr = (unsigned char*) FROM;										\
            nOut = 0;														\
            nWritten = 0;													\
            nRead = fread( (void*)TL_B64_encodingPtr, 1, SIZEBUF, FILEPATH );							\
            if( ferror( FILEPATH ) ){												\
                r = TL_B64_ERR;													\
                break;														\
            }															\
            if( nRead == 0 ){													\
                break;														\
			}													\
            nDiv = ((unsigned long)nRead) / 3;											\
            nRem = ((unsigned long)nRead) % 3;											\
            nChars = 0;														\
            while( nDiv > 0){													\
                TO[nOut+0] = base64_lookup[ (TL_B64_encodingPtr[0] >> 2) & 0x3f];						\
                TO[nOut+1] = base64_lookup[((TL_B64_encodingPtr[0] << 4) & 0x30) + ((TL_B64_encodingPtr[1] >> 4) & 0xf)];	\
                TO[nOut+2] = base64_lookup[((TL_B64_encodingPtr[1] << 2) & 0x3c) + ((TL_B64_encodingPtr[2] >> 6) & 0x3)];	\
                TO[nOut+3] = base64_lookup[  TL_B64_encodingPtr[2] & 0x3f];							\
                TL_B64_encodingPtr += 3;											\
                nOut += 4;													\
                nDiv--;														\
                nChars += 4;													\
                if( nChars >= TL_B64_LINE_CHARS && TL_B64_LINE_CHARS != 0 ){							\
                    nChars = 0;													\
                    if( TL_B64_fUseCRLF ){											\
                        TO[nOut++] = '\r';											\
                    }														\
                    TO[nOut++] = '\n';												\
                }														\
            }															\
            switch( nRem ){													\
                case 2:														\
                    TO[nOut+0] = base64_lookup[ (TL_B64_encodingPtr[0] >> 2) & 0x3f];						\
                    TO[nOut+1] = base64_lookup[((TL_B64_encodingPtr[0] << 4) & 0x30) + ((TL_B64_encodingPtr[1] >> 4) & 0xf)];	\
                    TO[nOut+2] = base64_lookup[ (TL_B64_encodingPtr[1] << 2) & 0x3c];						\
                    TO[nOut+3] = '=';												\
                    nOut += 4;													\
                    nChars += 4;												\
                    if( nChars >= TL_B64_LINE_CHARS && TL_B64_LINE_CHARS != 0 ){						\
                        nChars = 0;												\
                        if( TL_B64_fUseCRLF ){											\
                            TO[nOut++] = '\r';											\
                        }													\
                        TO[nOut++] = '\n';											\
                    }														\
                    break;													\
                case 1:														\
                    TO[nOut+0] = base64_lookup[ (TL_B64_encodingPtr[0] >> 2) & 0x3f];						\
                    TO[nOut+1] = base64_lookup[ (TL_B64_encodingPtr[0] << 4) & 0x30];						\
                    TO[nOut+2] = '=';												\
                    TO[nOut+3] = '=';												\
                    nOut += 4;													\
                    nChars += 4;												\
                    if( nChars >= TL_B64_LINE_CHARS && TL_B64_LINE_CHARS != 0 ){						\
                        nChars = 0;												\
                        if( TL_B64_fUseCRLF ){											\
                            TO[nOut++] = '\r';											\
                        }													\
                        TO[nOut++] = '\n';											\
                    }														\
                    break;													\
            }															\
            if( nRem > 0 || feof( FILEPATH ) ){											\
                if( nChars > 0 ){												\
                    nChars = 0;													\
                    if( TL_B64_fUseCRLF ){											\
                        TO[nOut++] = '\r';											\
                    }														\
                    TO[nOut++] = '\n';												\
                }														\
            }															\
            nWritten = (size_t) (nOut);												\
            if( nWritten > 0 ){													\
                n = fwrite( (void*)TO, 1, nWritten, TL_B64_FILE_OUT_FD );							\
                if( ferror( TL_B64_FILE_OUT_FD ) || n < nWritten ){								\
                    r = TL_B64_ERR;												\
                    break;													\
                }														\
            }															\
            if( nRem > 0 || feof( FILEPATH ) ){											\
                break;														\
            }															\
        }															\
        break;															\
	}

#define TL_B64_ENCODE_FROM_BUFFER(FROM,TO,SIZEBUF)										\
        for(;;){														\
            nDiv = 0;														\
            nRem = 0;														\
            nChars = 0;														\
            TL_B64_encodingPtr = (unsigned char*) FROM;										\
            nOut = 0;														\
            nWritten = 0;													\
            nDiv = ((unsigned long)SIZEBUF) / 3;										\
            nRem = ((unsigned long)SIZEBUF) % 3;										\
            nChars = 0;														\
		n=0;														\
            while( nDiv > 0){													\
                TO[nOut+0] = base64_lookup[ (TL_B64_encodingPtr[0] >> 2) & 0x3f];						\
                TO[nOut+1] = base64_lookup[((TL_B64_encodingPtr[0] << 4) & 0x30) + ((TL_B64_encodingPtr[1] >> 4) & 0xf)];	\
                TO[nOut+2] = base64_lookup[((TL_B64_encodingPtr[1] << 2) & 0x3c) + ((TL_B64_encodingPtr[2] >> 6) & 0x3)];	\
                TO[nOut+3] = base64_lookup[  TL_B64_encodingPtr[2] & 0x3f];							\
                TL_B64_encodingPtr += 3;											\
                nOut += 4;													\
                nDiv--;														\
                nChars += 4;													\
                if( nChars >= TL_B64_LINE_CHARS && TL_B64_LINE_CHARS != 0 ){							\
                        nChars = 0;												\
                        if( TL_B64_fUseCRLF ){											\
                            TO[nOut++] = '\r';											\
                        }													\
                    TO[nOut++] = '\n';												\
                }														\
            }															\
            switch( nRem ){													\
                case 2:														\
                    TO[nOut+0] = base64_lookup[ (TL_B64_encodingPtr[0] >> 2) & 0x3f];						\
                    TO[nOut+1] = base64_lookup[((TL_B64_encodingPtr[0] << 4) & 0x30) + ((TL_B64_encodingPtr[1] >> 4) & 0xf)];	\
                    TO[nOut+2] = base64_lookup[ (TL_B64_encodingPtr[1] << 2) & 0x3c];						\
                    TO[nOut+3] = '=';												\
                    nOut += 4;													\
                    nChars += 4;												\
                    if( nChars >= TL_B64_LINE_CHARS && TL_B64_LINE_CHARS != 0 ){						\
                        nChars = 0;												\
                        if( TL_B64_fUseCRLF ){											\
                            TO[nOut++] = '\r';											\
                        }													\
                        TO[nOut++] = '\n';											\
                    }														\
                    break;													\
                case 1:														\
                    TO[nOut+0] = base64_lookup[ (TL_B64_encodingPtr[0] >> 2) & 0x3f];						\
                    TO[nOut+1] = base64_lookup[ (TL_B64_encodingPtr[0] << 4) & 0x30];						\
                    TO[nOut+2] = '=';												\
                    TO[nOut+3] = '=';												\
                    nOut += 4;													\
                    nChars += 4;												\
                    if( nChars >= TL_B64_LINE_CHARS && TL_B64_LINE_CHARS != 0 ){						\
                        nChars = 0;												\
                        if( TL_B64_fUseCRLF ){											\
                            TO[nOut++] = '\r';											\
                        }													\
                        TO[nOut++] = '\n';											\
                    }														\
                    break;													\
            }															\
            if( nRem > 0 ){													\
                if( nChars > 0 ){												\
                    nChars = 0;													\
                    if( TL_B64_fUseCRLF ){											\
                        TO[nOut++] = '\r';											\
                    }														\
                    TO[nOut++] = '\n';												\
                }														\
            }															\
		if(nOut >= SIZEBUF ){break;}											\
        }

#define TL_B64_DECODE_FROM_FD(FROMFILEPATH,FILEPATH)                    \
    while(1){                                                           \
        bits = 'z';                                                     \
        nRead = fread( (void*)&c, 1, 1, FROMFILEPATH );                 \
        if( ferror( FROMFILEPATH )){                                    \
            r = TL_B64_ERR;                                             \
            break;                                                      \
        }                                                               \
        if( nRead == 0 ){                                               \
            break;                                                      \
        }                                                               \
        else if( c >= 'A' && c <= 'Z' ){                                \
            bits = (unsigned char) (c - 'A');                           \
        }                                                               \
        else if( c >= 'a' && c <= 'z' ){                                \
            bits = (unsigned char) (c - 'a' + (char)26);                \
        }                                                               \
        else if( c >= '0' && c <= '9' ){                                \
            bits = (unsigned char) (c - '0' + (char)52);                \
        }                                                               \
        else if( c == '+' ){                                            \
            bits = (unsigned char) 62;                                  \
        }                                                               \
        else if( c == '/' ){                                            \
            bits = (unsigned char) 63;                                  \
        }                                                               \
        else if( c == '-' ){                                            \
            z = 1;                                                      \
        }                                                               \
        else if( c == '=' ){                                            \
            break;                                                      \
        }                                                               \
        else{                                                           \
            bits = (unsigned char) 'y';                                 \
        }                                                               \
        if( bits < (unsigned char) 64 ){                                \
            switch(nData++){                                            \
                case 0:                                                 \
                    data[0] = (bits << 2) & 0xfc;                       \
                    break;                                              \
                case 1:                                                 \
                    data[0] |= (bits >> 4) & 0x03;                      \
                    data[1] = (bits << 4) & 0xf0;                       \
                    break;                                              \
                case 2:                                                 \
                    data[1] |= (bits >> 2) & 0x0f;                      \
                    data[2] = (bits << 6) & 0xc0;                       \
                    break;                                              \
                case 3:                                                 \
                    data[2] |= bits & 0x3f;                             \
                    break;                                              \
            }                                                           \
            if( nData == 4 ){                                           \
                n = fwrite( (void*)data, 1, 3, FILEPATH );              \
                if( ferror( FILEPATH ) || n < 3 ){                      \
                    r = TL_B64_ERR;                                     \
                    break;                                              \
                }                                                       \
                nData = 0;                                              \
            }                                                           \
        }                                                               \
        if(feof( TL_B64_FILE_IN_FD )){                                  \
            break;                                                      \
        }                                                               \
    }                                                                   \
    if( r == TL_B64_OK && nData > 0 ){                                  \
        if( nData == 1 ){                                               \
            r = TL_B64_ERR;                                             \
        }                                                               \
        else{                                                           \
            n = fwrite( (void*)data, 1, nData-1, FILEPATH );            \
            if( ferror( FILEPATH ) || n < (nData-1) ){                  \
                r = TL_B64_ERR;                                         \
            }                                                           \
        }                                                               \
    }

#define TL_B64_DECODE_WITH_BUFFER(FROM,TO,BUFFERSIZE)           \
    nData=0;                                                    \
    TL_B64_DECODE_COUNTER_INPUT=0;                              \
    TL_B64_DECODE_COUNTER_OUTPUT=0;                             \
    while(1){                                                   \
        if(TL_B64_DECODE_COUNTER_INPUT>=BUFFERSIZE){            \
            break;                                              \
        }                                                       \
        c = FROM[TL_B64_DECODE_COUNTER_INPUT];                  \
        TL_B64_DECODE_COUNTER_INPUT++;                          \
        bits = 'z';                                             \
        if( c >= 'A' && c <= 'Z' ){                             \
            bits = (unsigned char) (c - 'A');                   \
        }                                                       \
        else if( c >= 'a' && c <= 'z' ){                        \
            bits = (unsigned char) (c - 'a' + (char)26);        \
        }                                                       \
        else if( c >= '0' && c <= '9' ){                        \
            bits = (unsigned char) (c - '0' + (char)52);        \
        }                                                       \
        else if( c == '+' ){                                    \
            bits = (unsigned char) 62;                          \
        }                                                       \
        else if( c == '/' ){                                    \
            bits = (unsigned char) 63;                          \
        }                                                       \
        else if( c == '-' ){                                    \
            z = 1;                                              \
        }                                                       \
        else if( c == '=' ){                                    \
            break;                                              \
        }                                                       \
        else{                                                   \
            bits = (unsigned char) 'y';                         \
        }                                                       \
        if( bits < (unsigned char) 64 ){                        \
            switch(nData++){                                    \
                case 0:                                         \
                    data[0] = (bits << 2) & 0xfc;               \
                    break;                                      \
                case 1:                                         \
                    data[0] |= (bits >> 4) & 0x03;              \
                    data[1] = (bits << 4) & 0xf0;               \
                    break;                                      \
                case 2:                                         \
                    data[1] |= (bits >> 2) & 0x0f;              \
                    data[2] = (bits << 6) & 0xc0;               \
                    break;                                      \
                case 3:                                         \
                    data[2] |= bits & 0x3f;                     \
                    break;                                      \
            }                                                   \
            if( nData == 4 ){                                   \
                TO[TL_B64_DECODE_COUNTER_OUTPUT]=data[0];      \
                TO[TL_B64_DECODE_COUNTER_OUTPUT+1]=data[1];     \
                TO[TL_B64_DECODE_COUNTER_OUTPUT+2]=data[2];     \
                TL_B64_DECODE_COUNTER_OUTPUT+=3;                \
                nData = 0;                                      \
            }                                                   \
        }                                                       \
    }                                                           \
    if( nData > 0 ){                                            \
        if( nData == 1 ){                                       \
            r = TL_B64_ERR;                                     \
        }                                                       \
        else if(nData == 2){                                    \
                TO[TL_B64_DECODE_COUNTER_OUTPUT]=data[0];       \
        }                                                       \
        else if(nData == 3){                                    \
                TO[TL_B64_DECODE_COUNTER_OUTPUT]=data[0];       \
                TO[TL_B64_DECODE_COUNTER_OUTPUT+1]=data[1];     \
        }                                                       \
    }

///////////////////////////////////////////////////////////////
// New UDP / TCP Implementation
#define TL_IPv4_PORT_DEFAULT	1337

#if WINDOWS
	#define TL_IPv4_INIT()                      \
		WORD wVersionRequested = MAKEWORD(1,1);	\
		WSADATA wsaData;						\
		WSAStartup(wVersionRequested, &wsaData)
#elif NOTWINDOWS
	#define TL_IPv4_INIT()	;
#endif

#define	TL_IPv4_SOCKET_INIT(NAME)               \
    int                  NAME;                  \
    struct sockaddr_in   NAME##_addr;			\
    struct in_addr       NAME##_ip_addr;		\
    int                  NAME##_addr_len;		\
    int                  NAME##_retcode

#define TL_IPv4_SOCKET_UDP_DEFAULTS(NAME)				\
		NAME##_addr.sin_family = AF_INET;			\
		NAME##_addr.sin_port = htons(TL_IPv4_PORT_DEFAULT);	\
		NAME##_addr.sin_addr.s_addr = htonl(INADDR_ANY)

#define TL_IPv4_SOCKET_UDP_INIT(NAME)				\
	NAME = socket(AF_INET, SOCK_DGRAM, 0);			\
	if( NAME < 0){									\
		printf("\n[TL ERROR] socket() failed \n");	\
		perror("\n\tCaused by :");					\
		exit(-1);									\
	}												\
	TL_IPv4_SOCKET_UDP_DEFAULTS(NAME)

#define TL_IPv4_SOCKET_SETPORT(NAME,PORT)	NAME##_addr.sin_port = htons(PORT)

#define TL_IPv4_SOCKET_SETIP(NAME,IP)   NAME##_addr.sin_addr.s_addr = inet_addr(IP)

#define TL_IPv4_SOCKET_SETIP_INANY(NAME)	NAME##_addr.sin_addr.s_addr = htonl(INADDR_ANY)

#define	TL_IPv4_SOCKET_BIND(NAME)	\
	NAME##_retcode = bind(NAME, (struct sockaddr *)&NAME##_addr,sizeof(NAME##_addr)); if (NAME##_retcode < 0){printf("\n[TL ERROR] bind() failed");perror("\n\tCaused by");exit(-1);}

#define TL_IPv4_SOCKET_UDP_RECVFROM(NAME,BUFFER)	\
  NAME##_addr_len = sizeof(NAME##_addr);				\
  NAME##_retcode = recvfrom(NAME, BUFFER, sizeof(BUFFER), 0,(struct sockaddr *) &NAME##_addr, &NAME##_addr_len); if(NAME##_retcode < 0){printf("\n[TL ERROR] recvfrom() 2respond failed");perror("\n\tCaused by");exit(-1);}

#define TL_TPv4_SOCKET_UDP_RECVFROM_2RESPOND(NAME,BUFFER,SENDTO)	\
  NAME##_addr_len = sizeof(SENDTO##_addr);				\
  NAME##_retcode = recvfrom(NAME, BUFFER, sizeof(BUFFER), 0,(struct sockaddr *) &SENDTO##_addr, &NAME##_addr_len); if(NAME##_retcode < 0){printf("\n[TL ERROR] recvfrom() 2respond failed");perror("\n\tCaused by");exit(-1);}

#define TL_IPv4_SOCKET_UDP_SENDTO_2RESPOND(SENDTO,BUFFER,SIZE,NAME) NAME##_retcode = sendto(NAME, BUFFER, SIZE, 0,(struct sockaddr *)&SENDTO##_addr, sizeof(SENDTO##_addr)); if (NAME##_retcode < 0){printf("\n[ERROR] sendto() failed!\n");perror("\n\tCaused by");exit(-1);}

#define TL_IPv4_SOCKET_UDP_SENDTO(NAME,BUFFER,SIZE) NAME##_retcode = sendto(NAME, BUFFER, SIZE, 0,(struct sockaddr *)&NAME##_addr, sizeof(NAME##_addr)); if (NAME##_retcode < 0){printf("\n[ERROR] sendto() failed!\n");perror("\n\tCaused by");exit(-1);}

#if WINDOWS
#define	TL_IPv4_SOCKET_CLOSE(NAME)																					\
                NAME##_retcode = closesocket( NAME );																		\
				if (NAME##_retcode < 0){printf("\n[TL ERROR] closesocket() failed!\n");perror("\n\tCaused by");exit(-1);}

#elif NOTWINDOWS
#define	TL_IPv4_SOCKET_CLOSE(NAME)																					\
                NAME##_retcode = close( NAME );																		\
				if (NAME##_retcode < 0){printf("\n[TL ERROR] close() failed!\n");perror("\n\tCaused by");exit(-1);}
#endif

#if WINDOWS
        #define TL_IPv4_Cleanup()	WSACleanup()
#elif NOTWINDOWS
        #define TL_IPv4_Cleanup()	;
#endif

#define TL_IPv4_SOCKET_TCP_DEFAULTS(NAME)              	\
    NAME##_addr.sin_family = AF_INET;			\
    NAME##_addr.sin_port = htons(TL_IPv4_PORT_DEFAULT); \
    NAME##_addr.sin_addr.s_addr = htonl(INADDR_ANY)

#define	TL_IPv4_SOCKET_TCP_INIT(NAME)                               \
    NAME = socket(AF_INET, SOCK_STREAM, 0);                         \
    if( NAME < 0){                                                  \
        TL_DEBUGGING_ENABLE;                                           \
        TL_DEBUGF(DEBUGGING_FILE_FD,"\n[TL ERROR] socket() failed!\n");	\
        perror("\n\tCaused by");                                    \
        TL_DEBUGGING_DISABLE;                                          \
        exit(-1);                                                   \
    }                                                               \
    TL_IPv4_SOCKET_TCP_DEFAULTS(NAME)

#define	TL_IPv4_SOCKET_TCP_CONNECT(NAME)						\
    NAME##_retcode= connect(NAME, (struct sockaddr *)&NAME##_addr,sizeof(NAME##_addr));	\
    if(NAME##_retcode < 0){								\
        TL_DEBUGGING_ENABLE;								\
        TL_DEBUGF(DEBUGGING_FILE_FD,"\n[TL ERROR] connect() failed!\n");			\
        TL_DEBUGGING_DISABLE;								\
        exit(-1);									\
    }

#define	TL_IPv4_SOCKET_TCP_RECEIVE(NAME,MSG,SIZE)						\
NAME##_retcode = recv(NAME, MSG, SIZE, 0);				\
if (NAME##_retcode < 0){							\
    TL_DEBUGGING_ENABLE;						\
    TL_DEBUGF(DEBUGGING_FILE_FD,"\n[TL ERROR] TCP recv() failed!\n");	\
    TL_DEBUGGING_DISABLE;						\
    exit(-1);							\
}

#define	TL_IPv4_SOCKET_TCP_SEND(NAME,MSG,SIZE)						\
NAME##_retcode = send(NAME, MSG, SIZE, 0);				\
if (NAME##_retcode < 0){							\
    TL_DEBUGGING_ENABLE;						\
    TL_DEBUGF(DEBUGGING_FILE_FD,"\n[TL ERROR] TCP send() failed!\n");	\
    TL_DEBUGGING_DISABLE;						\
    exit(-1);							\
}

#define	TL_IPv4_SOCKET_TCP_BIND_and_LISTEN_INIT(NAME,NUM)								\
NAME##_retcode = bind(NAME, (struct sockaddr *)&NAME##_addr,sizeof(NAME##_addr));	\
if (NAME##_retcode < 0){										\
    TL_DEBUGGING_ENABLE;									\
    TL_DEBUGF(DEBUGGING_FILE_FD,"\n[TL ERROR] TCP bind() failed!\n");				\
    TL_DEBUGGING_DISABLE;									\
    exit(-1);										\
}												\
listen(NAME, NUM)

#define TL_IPv4_SOCKET_TCP_ACCEPT(NAME,RESPONDER)                   \
    RESPONDER##_addr_len = sizeof(RESPONDER##_addr);                \
    RESPONDER = accept(NAME, (struct sockaddr *)&RESPONDER##_addr, &RESPONDER##_addr_len);if (RESPONDER < 0){printf("\n[TL ERROR] accept() failed! \n");exit(-1);}

#if WINDOWS
	#define TL_THREADING_INIT(NAME)	\
		int TL_StackSize=4096;	\
		int NAME##_TL_retcode
#elif NOTWINDOWS
	#define TL_THREADING_INIT(NAME)			\
		pthread_t	NAME##_thread_id;	\
		int NAME##_TL_retcode

#endif

///////////////////////////////////////////////////////////////
// Threading Template

#if WINDOWS
	#define TL_THREADING_START(NAME,FUNCTION,CONTENT) 				\
		NAME##_TL_retcode = _beginthread(FUNCTION, TL_StackSize, CONTENT);	\
    		if(NAME##_TL_retcode < 0){						\
      			printf("\n[TL ERROR] Unable to create a thread!");		\
      			exit(1);							\
 		}
#endif
#if NOTWINDOWS
	#define TL_THREADING_START(NAME,FUNCTION,CONTENT) 						\
		NAME##_TL_retcode = pthread_create(&NAME##_thread_id, NULL, FUNCTION , CONTENT);	\
    		if(NAME##_TL_retcode!=0){								\
      			printf("\n[TL ERROR] Unable to create a thread!");				\
      			exit(1);									\
 		}
#endif

#if WINDOWS
	#define TL_THREADING_STOP()	_endthread()
#endif
#if NOTWINDOWS
	#define TL_THREADING_STOP()	pthread_exit(NULL)
#endif

//---------------------------------------------------
//  HTTP response messages
#define  TL_HTTP_BUF_SIZE	4096

#define TL_HTTP_INIT()						\
        TL_IPv4_SOCKET_INIT(TL_HTTP_client_s);			\
        char           TL_HTTP_in_buf[TL_HTTP_BUF_SIZE];	\
        char           TL_HTTP_out_buf[TL_HTTP_BUF_SIZE];	\
        int            TL_HTTP_buf_len;				\
        char           TL_HTTP_command[TL_HTTP_BUF_SIZE];	\
        char           TL_HTTP_file_name[TL_HTTP_BUF_SIZE];	\
        FILE *TL_HTTP_fh;					\
	int TL_HTTP_CONFIG_THREADING=0;				\
	int TL_HTTP_PORT = TL_IPv4_PORT_DEFAULT

#define	TL_HTTP_CONFIG_THREADING_ENABLED()    TL_HTTP_CONFIG_THREADING=1
#define	TL_HTTP_CONFIG_THREADING_DISABLED()   TL_HTTP_CONFIG_THREADING=0

#define TL_HTTP_OK_IMAGE  "HTTP/1.0 200 OK\r\nContent-Type:image/gif\r\n\r\n"
#define TL_HTTP_OK_TEXT   "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n"
#define TL_HTTP_NOTOK_404 "HTTP/1.0 404 Not Found\r\nContent-Type:text/html\r\n\r\n"
#define TL_HTTP_MESS_404  "<html><head><title>TrojanLib Weblite Server: 404 ERROR</title></head><body><br><br><center><h1>FILE NOT FOUND :'(</h1></center></body></html>"

#define TL_HTTP_GET_RESPOND(FD)                                                                 \
    TL_HTTP_client_s = FD;									\
    TL_IPv4_SOCKET_TCP_RECEIVE(TL_HTTP_client_s,TL_HTTP_in_buf, sizeof(TL_HTTP_in_buf));        \
    if (TL_HTTP_client_s_retcode <= 0){                                                         \
        printf("\n[WARNING] Receive failed --- Connection dropped or finished!\n");             \
        TL_IPv4_SOCKET_CLOSE(TL_HTTP_client_s);                                                 \
        if(TL_HTTP_CONFIG_THREADING==1)                                                         \
            TL_THREADING_STOP();                                                                \
    }                                                                                           \
    sscanf(TL_HTTP_in_buf, "%s %s \n", TL_HTTP_command, TL_HTTP_file_name);                     \
    if (strcmp(TL_HTTP_command, "GET") != 0){                                                   \
        printf("\n[ERROR] Not a GET --- received TL_HTTP_command = '%s' \n", TL_HTTP_command);  \
        TL_IPv4_SOCKET_CLOSE(TL_HTTP_client_s);                                                 \
        if(TL_HTTP_CONFIG_THREADING==1)                                                         \
            TL_THREADING_STOP();                                                                \
    }                                                                                           \
    TL_HTTP_fh = fopen (&TL_HTTP_file_name[1],"rb");                                            \
    if (TL_HTTP_fh == NULL ){                                                                   \
        printf("\nFile '%s' not found --- sending an HTTP 404", &TL_HTTP_file_name[1]);         \
        strcpy(TL_HTTP_out_buf, TL_HTTP_NOTOK_404);                                             \
        TL_IPv4_SOCKET_TCP_SEND(TL_HTTP_client_s,TL_HTTP_out_buf,strlen(TL_HTTP_out_buf));      \
        strcpy(TL_HTTP_out_buf, TL_HTTP_MESS_404);                                              \
        TL_IPv4_SOCKET_TCP_SEND(TL_HTTP_client_s,TL_HTTP_out_buf,strlen(TL_HTTP_out_buf));      \
        TL_IPv4_SOCKET_CLOSE(TL_HTTP_client_s);                                                 \
        if(TL_HTTP_CONFIG_THREADING==1)                                                         \
            TL_THREADING_STOP();                                                                \
    }                                                                                           \
    printf("\nSending file '%s'", &TL_HTTP_file_name[1]);                                       \
    if (strstr(TL_HTTP_file_name, ".gif") != NULL)                                              \
        strcpy(TL_HTTP_out_buf, TL_HTTP_OK_IMAGE);                                              \
    else                                                                                        \
        strcpy(TL_HTTP_out_buf, TL_HTTP_OK_TEXT);                                               \
    TL_IPv4_SOCKET_TCP_SEND(TL_HTTP_client_s,TL_HTTP_out_buf,strlen(TL_HTTP_out_buf));          \
    while(1){                                                                                   \
        TL_HTTP_buf_len = fread(TL_HTTP_out_buf,1,TL_HTTP_BUF_SIZE,TL_HTTP_fh);                 \
        if (TL_HTTP_buf_len <= 0) break;                                                        \
        TL_IPv4_SOCKET_TCP_SEND(TL_HTTP_client_s,TL_HTTP_out_buf,TL_HTTP_buf_len);              \
    }                                                                                           \
    fclose(TL_HTTP_fh)

#define TL_TIME_INIT()					\
	time_t TL_TIME_SYSTEM;				\
	struct tm * TL_TIME_SYSTEM_STRUCT;	\
	char TL_TIME_STRING_FULL[30];		\
	char TL_TIME_STRING_DATE[30];		\
	char TL_TIME_STRING_TIME[30]

#define TL_TIME_GET()																						\
	time(&TL_TIME_SYSTEM);																					\
	TL_TIME_SYSTEM_STRUCT = localtime(&TL_TIME_SYSTEM);														\
	strftime(TL_TIME_STRING_FULL,sizeof(TL_TIME_STRING_FULL),"%Y-%m-%dT%H:%M:%S",TL_TIME_SYSTEM_STRUCT);	\
	strftime(TL_TIME_STRING_DATE,sizeof(TL_TIME_STRING_DATE),"%Y-%m-%d",TL_TIME_SYSTEM_STRUCT);				\
	strftime(TL_TIME_STRING_TIME,sizeof(TL_TIME_STRING_TIME),"%H:%M:%S",TL_TIME_SYSTEM_STRUCT)

#ifndef TL_IPv4_SOCKET_TCP_NCP_SIZE
	#define TL_IPv4_SOCKET_TCP_NCP_SIZE	2061250
#endif

//Crazy File ops
#if _WIN64
	//#define fstat _fstati64 //Mingw-w64 has this defined on line 259 of stat.h
	#define stat _stat64
#endif

#define TL_IPv4_SOCKET_TCP_NCP_INIT()											\
	FILE 			*TL_IPv4_SOCKET_TCP_NCP_FILE;								\
	int 			TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE;						\
	off_t			TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE=0;							\
	off_t 			TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER=0;						\
	int             TL_IPv4_SOCKET_TCP_NCP_RETCODE;								\
	char 			TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER[500];			\
	struct stat 	TL_IPv4_SOCKET_TCP_NCP_FILESTAT;							\
	char	TL_IPv4_SOCKET_TCP_NCP_BUFFER[TL_IPv4_SOCKET_TCP_NCP_SIZE]


#if WINDOWS
	#define	TL_IPv4_SOCKET_TCP_NCP_SEND(SOCKET_SEND,PASSED_INPUT_NAME)																													\
		TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE = open( PASSED_INPUT_NAME , O_BINARY | O_RDONLY); if(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE < -1){fputs ("File error",stderr); exit (1);}\
		fstat( TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE ,&TL_IPv4_SOCKET_TCP_NCP_FILESTAT);\
		sprintf(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER,"%20jd",TL_IPv4_SOCKET_TCP_NCP_FILESTAT.st_size);\
		TL_IPv4_SOCKET_TCP_SEND(SOCKET_SEND,TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER,strlen(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER));      \
		TL_DEBUGF(stdout,"\n\tSending file length to receiver: %20jd",TL_IPv4_SOCKET_TCP_NCP_FILESTAT.st_size);		\
		TL_DEBUGF(stdout,"\n\tReading File in chunks of: %i\n",sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER));		\
		TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_SEND, TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER));									\
		TL_DEBUGF(stdout,"\n\tStarting to send file...");		\
		do{		\
			TL_IPv4_SOCKET_TCP_NCP_RETCODE = read(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE,TL_IPv4_SOCKET_TCP_NCP_BUFFER,sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER));\
			TL_IPv4_SOCKET_TCP_SEND(SOCKET_SEND,TL_IPv4_SOCKET_TCP_NCP_BUFFER,TL_IPv4_SOCKET_TCP_NCP_RETCODE);																			\
		}while(TL_IPv4_SOCKET_TCP_NCP_RETCODE>0);\
		close(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE);\
		TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_SEND, TL_IPv4_SOCKET_TCP_NCP_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER))
#endif

#if NOTWINDOWS
	#define	TL_IPv4_SOCKET_TCP_NCP_SEND(SOCKET_SEND,PASSED_INPUT_NAME)																													\
		TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE = open( PASSED_INPUT_NAME , O_RDONLY); if(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE < -1){fputs ("File error",stderr); exit (1);}\
		fstat( TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE ,&TL_IPv4_SOCKET_TCP_NCP_FILESTAT);\
		sprintf(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER,"%20jd",TL_IPv4_SOCKET_TCP_NCP_FILESTAT.st_size);\
		TL_IPv4_SOCKET_TCP_SEND(SOCKET_SEND,TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER,strlen(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER));      \
		TL_DEBUGF(stdout,"\n\tSending file length to receiver: %20jd",TL_IPv4_SOCKET_TCP_NCP_FILESTAT.st_size);		\
		TL_DEBUGF(stdout,"\n\tReading File in chunks of: %i\n",sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER));		\
		TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_SEND, TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER));									\
		TL_DEBUGF(stdout,"\n\tStarting to send file...");		\
		do{		\
			TL_IPv4_SOCKET_TCP_NCP_RETCODE = read(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE,TL_IPv4_SOCKET_TCP_NCP_BUFFER,sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER));\
			TL_IPv4_SOCKET_TCP_SEND(SOCKET_SEND,TL_IPv4_SOCKET_TCP_NCP_BUFFER,TL_IPv4_SOCKET_TCP_NCP_RETCODE);																			\
		}while(TL_IPv4_SOCKET_TCP_NCP_RETCODE>0);\
		close(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE);\
		TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_SEND, TL_IPv4_SOCKET_TCP_NCP_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER))
#endif
	//		TL_DEBUGF(stdout,"\n\tRead in: %i bytes, Sent: %i\n",TL_IPv4_SOCKET_TCP_NCP_RETCODE, SOCKET_SEND##_retcode);		\

	
//	
//	fseek(TL_IPv4_SOCKET_TCP_NCP_FILE , 0 , SEEK_END); TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE = (long long)ftell(TL_IPv4_SOCKET_TCP_NCP_FILE); fseek (TL_IPv4_SOCKET_TCP_NCP_FILE , 0 , SEEK_SET);	\
//
#if WINDOWS
	#define	TL_IPv4_SOCKET_TCP_NCP_RECV(SOCKET_RECV,PASSED_OUTPUT_NAME)																			\
		TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER=0;																												\
		TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE = open(PASSED_OUTPUT_NAME ,O_BINARY | O_WRONLY | O_CREAT,0664); if(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE < -1){fputs ("File error",stderr); exit (1);}\
		TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_RECV, TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER));			\
		TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE = atoll(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER);																\
		TL_DEBUGF(stdout,"\nThis is the File Size incoming: %20jd\n",TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE);\
		TL_IPv4_SOCKET_TCP_SEND(SOCKET_RECV,"OK",3);      \
		do{																		\
			TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_RECV, TL_IPv4_SOCKET_TCP_NCP_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER));									\
			TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER+=(off_t)SOCKET_RECV##_retcode;																				\
			write(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE,TL_IPv4_SOCKET_TCP_NCP_BUFFER,SOCKET_RECV##_retcode);													\
		}while(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER < TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE && SOCKET_RECV##_retcode>0);																		\
		close(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE);\
		TL_IPv4_SOCKET_TCP_SEND(SOCKET_RECV,"OK",3)
#endif

#if NOTWINDOWS
	#define	TL_IPv4_SOCKET_TCP_NCP_RECV(SOCKET_RECV,PASSED_OUTPUT_NAME)																			\
		TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER=0;																												\
		TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE = open(PASSED_OUTPUT_NAME ,O_WRONLY | O_CREAT,0664); if(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE < -1){fputs ("File error",stderr); exit (1);}\
		TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_RECV, TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER));			\
		TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE = atoll(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER_BUFFER);																\
		TL_DEBUGF(stdout,"\nThis is the File Size incoming: %20jd\n",TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE);\
		TL_IPv4_SOCKET_TCP_SEND(SOCKET_RECV,"OK",3);      \
		do{																		\
			TL_IPv4_SOCKET_TCP_RECEIVE(SOCKET_RECV, TL_IPv4_SOCKET_TCP_NCP_BUFFER, sizeof(TL_IPv4_SOCKET_TCP_NCP_BUFFER));									\
			TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER+=(off_t)SOCKET_RECV##_retcode;																				\
			write(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE,TL_IPv4_SOCKET_TCP_NCP_BUFFER,SOCKET_RECV##_retcode);													\
		}while(TL_IPv4_SOCKET_TCP_NCP_FILE_COUNTER < TL_IPv4_SOCKET_TCP_NCP_FILE_SIZE && SOCKET_RECV##_retcode>0);																		\
		close(TL_IPv4_SOCKET_TCP_NCP_FILE_RETCODE);\
		TL_IPv4_SOCKET_TCP_SEND(SOCKET_RECV,"OK",3)
#endif


	//		TL_DEBUGF(stdout,"\n\tReceived: %20jd bytes\n",(off_t)SOCKET_RECV##_retcode);		\
	
//Because lol:
// https://www.securecoding.cert.org/confluence/display/seccode/FIO19-C.+Do+not+use+fseek()+and+ftell()+to+compute+the+size+of+a+regular+file
// https://www.securecoding.cert.org/confluence/display/cplusplus/FIO03-CPP.+Do+not+make+assumptions+about+fopen%28%29+and+file+creation

#define TL_IPv4_STR_CHECKING_VARS()							 	\
	char *TL_IPv4_Parsing_IP_Ptr,*TL_IPv4_Parsing_Port_Ptr;		\
	char TL_IPv4_Octet_str[4];									\
	int TL_IPv4_dot_counter=0;									\
	int TL_IPv4_Octet_len=0;									\
	int TL_IPv4_Colon_found=0;									\
	int TL_IPv4_Port_Valid=0;									\
	int TL_IPv4_IP_Valid=0;										\
	int TL_IPv4_STR_CHECKING_Counter=0

#define TL_IPv4_PORT_CHECK_STR(PortString,Exit_on_Fail)									\
	if(atoi(PortString)>65535 || atoi(PortString)<=0){									\
		printf("\n\n[ERROR] Port invalid! Port %i is out of range!\n",atoi(PortString));\
		if(Exit_on_Fail == 1){															\
			exit(EXIT_FAILURE);															\
		}																				\
	}																					\
	else{																				\
		TL_IPv4_Port_Valid=1;															\
	}

#define TL_IPv4_IP_CHECK_STR( IPString , Exit_on_Fail )																				\
	  if(strlen( IPString )<16 && strlen( IPString )>6){																			\
		TL_IPv4_IP_Valid=1;																											\
		for(TL_IPv4_Octet_len=0;TL_IPv4_Octet_len<4;TL_IPv4_Octet_len++){															\
			TL_IPv4_Octet_str[TL_IPv4_Octet_len]='\0';																				\
		}																															\
		TL_IPv4_Octet_len=0;																										\
		for(TL_IPv4_STR_CHECKING_Counter=0;TL_IPv4_STR_CHECKING_Counter<strlen(IPString);TL_IPv4_STR_CHECKING_Counter++){			\
			if( IPString[TL_IPv4_STR_CHECKING_Counter]=='.'){																		\
				TL_IPv4_dot_counter++;																								\
				TL_IPv4_Octet_str[TL_IPv4_Octet_len]='\0';																			\
				TL_IPv4_Octet_len=0;																								\
				if(atoi(TL_IPv4_Octet_str)>255 || atoi(TL_IPv4_Octet_str)<0){														\
					printf("\n\n[ERROR] IP Octet Formatting invalid! Octet[%i] value (%i) out of range!\n",TL_IPv4_dot_counter,atoi(TL_IPv4_Octet_str));			\
					if(Exit_on_Fail == 1){																							\
						exit(EXIT_FAILURE);																							\
					}																												\
					TL_IPv4_IP_Valid=0;																								\
				}																													\
				for(TL_IPv4_Octet_len=0;TL_IPv4_Octet_len<4;TL_IPv4_Octet_len++){													\
					TL_IPv4_Octet_str[TL_IPv4_Octet_len]='\0';																		\
				}																													\
				TL_IPv4_Octet_len=0;																								\
			}																														\
			else{																													\
				TL_IPv4_Octet_str[TL_IPv4_Octet_len]=IPString[TL_IPv4_STR_CHECKING_Counter];										\
				TL_IPv4_Octet_len++;																								\
			}																														\
			if(TL_IPv4_Octet_len>3){																								\
				printf("\n\n[ERROR] IP Octet Formatting invalid! Too many numbers in octet[%i]!\n",TL_IPv4_dot_counter+1);			\
				if(Exit_on_Fail == 1){																								\
					exit(EXIT_FAILURE);																								\
				}																													\
				TL_IPv4_IP_Valid=0;																									\
			}																														\
		}																															\
		if(atoi(TL_IPv4_Octet_str)>255 || atoi(TL_IPv4_Octet_str)<0){																\
			printf("\n\n[ERROR] IP Octet Formatting invalid! Octet[%i] value (%i) out of range!\n",TL_IPv4_dot_counter,atoi(TL_IPv4_Octet_str));					\
			if(Exit_on_Fail == 1){																									\
				exit(EXIT_FAILURE);																									\
			}																														\
			TL_IPv4_IP_Valid=0;																										\
		}																															\
		if(TL_IPv4_dot_counter!=3){																									\
			printf("\n\n[ERROR] IP Octet Formatting invalid! Check Octet, dot separators!\n");										\
			if(Exit_on_Fail == 1){																									\
				exit(EXIT_FAILURE);																									\
			}																														\
			TL_IPv4_IP_Valid=0;																									\
		}																															\
	  }																																\
	  else{																															\
		printf("\n\n[ERROR] IP String Length is invalid!\n");																		\
		if(Exit_on_Fail == 1){																										\
			exit(EXIT_FAILURE);																										\
		}																															\
		TL_IPv4_IP_Valid=0;																											\
	  }
 
#define TL_IPv4_PARSE_IP_AND_PORT(CombinedStr,IP_Dest,Port_Dest)																			\
	for(TL_IPv4_STR_CHECKING_Counter=0;TL_IPv4_STR_CHECKING_Counter<strlen(CombinedStr);TL_IPv4_STR_CHECKING_Counter++){					\
		if(CombinedStr[TL_IPv4_STR_CHECKING_Counter]==':'){																					\
			TL_IPv4_Colon_found=1;																											\
		}																																	\
	}																																		\
	TL_IPv4_Parsing_IP_Ptr = strtok(CombinedStr,":");																						\
	for(TL_IPv4_STR_CHECKING_Counter=0;TL_IPv4_STR_CHECKING_Counter<strlen(TL_IPv4_Parsing_IP_Ptr);TL_IPv4_STR_CHECKING_Counter++){			\
		IP_Dest[TL_IPv4_STR_CHECKING_Counter]=TL_IPv4_Parsing_IP_Ptr[TL_IPv4_STR_CHECKING_Counter];											\
	}																																		\
	IP_Dest[strlen(TL_IPv4_Parsing_IP_Ptr)]='\0';																							\
	if(TL_IPv4_Colon_found==1){																												\
		TL_IPv4_Parsing_Port_Ptr = strtok(NULL, ":");																						\
		for(TL_IPv4_STR_CHECKING_Counter=0;TL_IPv4_STR_CHECKING_Counter<strlen(TL_IPv4_Parsing_Port_Ptr);TL_IPv4_STR_CHECKING_Counter++){	\
			Port_Dest[TL_IPv4_STR_CHECKING_Counter]=TL_IPv4_Parsing_Port_Ptr[TL_IPv4_STR_CHECKING_Counter];									\
		}																																	\
		Port_Dest[strlen(TL_IPv4_Parsing_Port_Ptr)]='\0';																					\
	}
