#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Pre-processor instructions for Windows 
#ifdef WIN32

#include <winsock2.h> 

// Pre-processor instructions for Linux
#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Data structures and constants used in Windows
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif


/*
 * Windows sockets functions
 */
static void init(void)
{
#ifdef WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

static void end(void)
{
#ifdef WIN32
    WSACleanup();
#endif
}


// Debug & Logging function
void error(const char *msg)
{
    #ifdef DEBUG
    perror(msg);
    #else
    printf("Either the server shut down or the other player disconnected.\nGame over.\n");
    #endif 

    exit(0);
}

/*
 * Socket Read Functions
 */

/* Reads a message from the server socket. */
void recv_msg(int sockfd, char * msg)
{
    /* All messages are 3 bytes. */
    memset(msg, 0, 4);
    int n = recv(sockfd, msg, 3, 0);
    
    if (n < 0 || n != 3) /* Not what we were expecting. Server got killed or the other client disconnected. */ 
        error("ERROR reading message from server socket.");

    #ifdef DEBUG
    printf("[DEBUG] Received message: %s\n", msg);
    #endif 
}

/* Reads an int from the server socket. */
int recv_int(int sockfd)
{
    char msg;
    int n = recv(sockfd, &msg, sizeof(char), 0);
    
    if (n < 0 || n != sizeof(char)) 
        error("ERROR reading int from server socket");
    
    #ifdef DEBUG
    printf("[DEBUG] Received int: %d\n", msg - '0');
    #endif 
    
    return msg - '0';
}

/*
 * Socket Write Functions
 */

/* Writes an int to the server socket. */
void write_server_int(int sockfd, int msg)
{
    char castedMsg = msg + '0';
    int n = send(sockfd, &castedMsg, sizeof(char), 0);
    if (n < 0)
        error("ERROR writing int to server socket");
    
    #ifdef DEBUG
    printf("[DEBUG] Wrote int to server: %d\n", msg);
    #endif 
}

/*
 * Connect Functions
 */

/* Sets up the connection to the server. */
int connect_to_server(char * hostname, int portno)
{
    SOCKADDR_IN serv_addr;
    struct hostent *server;
 
    /* Get a socket. */
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
    if (sockfd == INVALID_SOCKET) 
        error("ERROR opening socket for server.");
	
    /* Get the address of the server. */
    server = gethostbyname(hostname);
	
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	
	/* Zero out memory for server info. */
	memset(&serv_addr, 0, sizeof(serv_addr));

	/* Set up the server info. */
    serv_addr.sin_family = AF_INET;
    memmove(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno); 

	/* Make the connection. */
    if (connect(sockfd, (SOCKADDR *) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) 
        error("ERROR connecting to server");

    #ifdef DEBUG
    printf("[DEBUG] Connected to server.\n");
    #endif 
    
    return sockfd;
}

/*
 * Game Functions
 */

/* Draws the game board to stdout. */
void draw_board(char board[][3])
{
    printf(" %c | %c | %c \n", board[0][0], board[0][1], board[0][2]);
    printf("-----------\n");
    printf(" %c | %c | %c \n", board[1][0], board[1][1], board[1][2]);
    printf("-----------\n");
    printf(" %c | %c | %c \n", board[2][0], board[2][1], board[2][2]);
}

/* Get's the players turn and sends it to the server. */
void take_turn(int sockfd)
{
    char buffer[10];
    
    while (1) { /* Ask until we receive. */ 
        printf("Enter 0-8 to make a move, or 9 for number of active players: ");
	    fgets(buffer, 10, stdin);
	    int move = buffer[0] - '0';
        if (move <= 9 && move >= 0){
            printf("\n");
            /* Send players move to the server. */
            write_server_int(sockfd, move);   
            break;
        } 
        else
            printf("\nInvalid input. Try again.\n");
    }
}

/* Gets a board update from the server. */
void get_update(int sockfd, char board[][3])
{
    /* Get the update. */
    int player_id = recv_int(sockfd);
    int move = recv_int(sockfd);

    /* Update the game board. */
    board[move/3][move%3] = player_id ? 'X' : 'O';    
}

/*
 * Main Program
 */

int main(int argc, char *argv[])
{
    init();
    /* Make sure host and port are specified. */
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    /* Connect to the server. */
    int sockfd = connect_to_server(argv[1], atoi(argv[2]));

    /* The client ID is the first thing we receive after connecting. */
    int id = recv_int(sockfd);

    #ifdef DEBUG
    printf("[DEBUG] Client ID: %d\n", id);
    #endif 

    char msg[4];
    char board[3][3] = { {' ', ' ', ' '}, /* Game board */
                         {' ', ' ', ' '}, 
                         {' ', ' ', ' '} };

    printf("Tic-Tac-Toe\n------------\n");

    /* Wait for the game to start. */
    do {
        recv_msg(sockfd, msg);
        if (!strcmp(msg, "HLD"))
            printf("Waiting for a second player...\n");
    } while ( strcmp(msg, "SRT") );

    /* The game has begun. */
    printf("Game on!\n");
    printf("Your are %c's\n", id ? 'X' : 'O');

    draw_board(board);

    while(1) {
        recv_msg(sockfd, msg);

        if (!strcmp(msg, "TRN")) { /* Take a turn. */
	        printf("Your move...\n");
	        take_turn(sockfd);
        }
        else if (!strcmp(msg, "INV")) { /* Move was invalid. Note that a "TRN" message will always follow an "INV" message, so we will end up at the above case in the next iteration. */
            printf("That position has already been played. Try again.\n"); 
        }
        else if (!strcmp(msg, "CNT")) { /* Server is sending the number of active players. Note that a "TRN" message will always follow a "CNT" message. */
            int num_players = recv_int(sockfd);
            printf("There are currently %d active players.\n", num_players); 
        }
        else if (!strcmp(msg, "UPD")) { /* Server is sending a game board update. */
            get_update(sockfd, board);
            draw_board(board);
        }
        else if (!strcmp(msg, "WAT")) { /* Wait for other player to take a turn. */
            printf("Waiting for other players move...\n");
        }
        else if (!strcmp(msg, "WIN")) { /* Winner. */
            printf("You win!\n");
            break;
        }
        else if (!strcmp(msg, "LSE")) { /* Loser. */
            printf("You lost.\n");
            break;
        }
        else if (!strcmp(msg, "DRW")) { /* Game is a draw. */
            printf("Draw.\n");
            break;
        }
        else /* Weird... */
            error("Unknown message.");
    }
    
    printf("Game over.\n");

    /* Close server socket and exit. */
    closesocket(sockfd);
    end();
    return 0;
}
