/*
 * tirc_client.c
 *
 * This will eventually be the client for the tirc server. It is currently
 * just code from an old project that has since been abandoned.
 *
 * Author: Jeffrey Picard
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT        65009

int main(int argc, char** argv) 
{
  char hostname[100];
  char buff[256];
  int sock, port;
  struct sockaddr_in sin;
  struct sockaddr_in pin;
  struct hostent *hp;

  if( argc < 4 )
  {
    perror("Error: to few arguments, need at least 3");
    exit( EXIT_FAILURE );
  }

  int i = 0;
  printf("argc = %d\n", argc);
  for( i = 0; i < argc; i++) 
    printf("[%d]: '%s'\n", i, argv[i]);

  // Get hostname, port and message from command line arguments
  if( *(argv[1]) == '-' )
    strcpy( hostname, "localhost" );
  else
    strcpy( hostname, argv[1] );

  if( *(argv[2]) == '-' )
    port = PORT;
  else
    port = atoi( argv[2] );

  if( *(argv[3]) == '-' )
    strcpy( buff, "8\n7" );
  else
    strcpy( buff, argv[3] );

  printf("looking up hostname '%s'\n", hostname);

  /* go find out about the desired host machine */
  if ((hp = gethostbyname(hostname)) == 0) 
  {
    perror("gethostbyname");
    exit(EXIT_FAILURE);
  }

  /* fill in the socket structure with host information */
  memset(&pin, 0, sizeof(pin));
  pin.sin_family = AF_INET;
  pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr_list)[0])->s_addr;
  pin.sin_port = htons(port);

  /* get TCP socket*/
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  int flag = 1;
  if( setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int) ) == -1 )
  {
    perror("option");
    exit( EXIT_FAILURE );
  }

  /* connect to port on HOST */
  if( connect(sock, (struct sockaddr *) &pin, sizeof(pin) ) == -1) 
  {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  printf("sending to %s %d\n", hostname, port);

  if( send( sock, argv[3], strlen( argv[3] ), 0) == -1 )
  {
    perror("send");
    exit( EXIT_FAILURE );
  }
  
  char c;
  //recv( sock, &c, 1, 0 );
  while( recv( sock, &c, sizeof(c), 0 ) != 0 )
  {
    printf("%c", c);
    recv( sock, &c, 1, 0 );
  }

  close( sock );
  exit( EXIT_SUCCESS );
}
