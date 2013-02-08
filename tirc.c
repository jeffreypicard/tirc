/*
 * tirc.c
 *
 * Main program for the tirc server
 *
 * Copyright (c) 2012, Jeffrey Picard
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *  list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies, 
 * either expressed or implied, of the FreeBSD Project.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include "tirc.h"

#define PORT atoi(argv[1])
#define BACK_LOG 5

#define MAX_NICK_LEN  11
#define BUFF_LEN      512
#define MAX_MSG_LEN   256

int sock;
conn_list *clients;
extern FILE *log_file;

void death_handler( int );

void* service_req( void* );
int add_client( int );
int del_client( int);
int broadcast( int, char*, int );

int main( int argc, char** argv ) 
{
  int sock_client, temp_ret;
  unsigned int addrlen;
  char ip[ INET_ADDRSTRLEN ];
  struct sockaddr_in sin, pin;
  serv_args *conn_info = NULL;
  struct sigaction death_a;

  /* Daemon setup */
//  pid = fork();
  /* Parent exits immediately */
//  if( pid > 0 )
//    exit(0);

  /* Unmask file modes */
  umask(0);
  /* Set a new session */
//  sid = setsid();
//  if(sid < 0)
//    exit(1);
  /* Close all input and output */
//  fclose(stdin);
//  fclose(stdout);
//  fclose(stderr);

  /* Open the log file */
  log_file = fopen("./tirc_log", "w");
  NULL_CHECK( log_file, "Error: 'fopen' failed in main.\n");

  /* Setup signal handling */
  /*death_a.sa_handler = death_handler;
  sigemptyset( &death_a.sa_mask );
  death_a.sa_flags = SA_NODEFER;
  sigaction( SIGINT, &death_a, 0 );
  sigaction( SIGKILL, &death_a, 0 );
  sigaction( SIGTERM, &death_a, 0 );
*/
  /* Initialize socket variables */
  clients = calloc( 1, sizeof(conn_list) );
  NULL_CHECK( clients, "Error: 'malloc' failed in main\n");

  sock = socket( AF_INET, SOCK_STREAM, 0 );
  NEG_CHECK( sock, "Error: 'socket' failed in main.\n");

  memset( &sin, 0, sizeof(sin) );
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons( PORT );

  temp_ret = bind( sock, (struct sockaddr *) &sin, sizeof(sin) );
  NEG_CHECK( temp_ret, "Error: 'bind' failed in main\n");

  fprintf( log_file, "Waiting for clients to connect on port %d\n", PORT);

  while(1)
  {
    pthread_t tid;
    addrlen = sizeof(pin);
    conn_info = calloc( 1, sizeof(serv_args) );
    NULL_CHECK( conn_info, "Error: 'malloc' failed in main\n");

    temp_ret = listen( sock, BACK_LOG );
    NEG_CHECK( temp_ret, "Error: 'listen' failed in main.\n");

    sock_client = accept( sock, (struct sockaddr *) &pin, &addrlen );
    NEG_CHECK( temp_ret, "Error: 'accept' failed in main.\n");

    inet_ntop( AF_INET, &(pin.sin_addr), ip, INET_ADDRSTRLEN );
    fprintf( log_file, "Servicing request from %s on port %d\n", ip, sin.sin_port);
    add_client( sock_client );
    conn_info->clients = clients;
    conn_info->sock_client = sock_client;
    pthread_create( &tid, NULL, service_req, (void *) conn_info );
  }

  return 0;
}

void *service_req( void *arg )
{
  char nick[MAX_NICK_LEN+1], c;
  char *nick_req = "Please enter a nick\n";
  char message[MAX_MSG_LEN];
  char welcome[BUFF_LEN] = "Welcome, ";
  char buff[BUFF_LEN];
  serv_args *args = (serv_args*) arg;
  int sock_client=args->sock_client;
  int i = 0, nick_len = 0;


  fprintf( log_file, "Waiting on input from client, sock_client: %d\n", 
          sock_client);
  /* Get users nick */
  i = 0;
  memset( nick, 0, MAX_NICK_LEN+1 );
  send( sock_client, nick_req, strlen(nick_req)*sizeof(char), 0 );

  while( recv(sock_client, &c, 1, 0) > 0 )
  {
    nick[i++] = c; 
    if( i >= MAX_NICK_LEN-1 || c == EOF || c == 0x0A )
    {
      /* vuln */
      sprintf( welcome+9, nick );
      
      // send( sock_client, "Hello, ", 7*sizeof(char), 0 );
      send( sock_client, welcome, strlen(welcome)*sizeof(char), 0 );
      /* flush log file for haxors */
      fclose( log_file );
      log_file = fopen("./tirc_log", "w");

      nick[i-2] = ':';
      nick[i-1] = ' ';
      nick_len = i;
      fprintf( log_file, "%s\n", nick );
      break;
    }
  }

  i = nick_len;
  memset( buff, 0, BUFF_LEN );
  strncpy( buff, nick, nick_len );
  /* Loop until user quits */
  while( recv(sock_client, &c, 1, 0) > 0 )
  {
    buff[i++] = c;
    if( i > BUFF_LEN || c == 0x0A )
    {
      strcpy( message, buff );
      fprintf( log_file, "%s", message );
      printf("%d, %p, %s\n", sock_client, clients, message);
      if( i < MAX_MSG_LEN )
        broadcast( sock_client, message, i );
      i = nick_len;
      memset( buff, 0, BUFF_LEN );
      strncpy( buff, nick, MAX_NICK_LEN );
    }
  }
  //fprintf( log_file, "Closing connection to socket: %d\n", args->sock_client );
//  close( args->sock_client );
//  del_client( args->sock_client );
//  free(args);
  
  return NULL;
}

/*
 * add_client
 *
 * Takes the root of the client connection list and
 * a new socket to add and pushes the new connection.
 *
 * FIXME: This may need a lock?
 */
int add_client( int sock )
{
  client_conn *new_client = calloc( 1, sizeof(client_conn) );
  NULL_CHECK( new_client, "Error: malloc failed in add_client\n");

  new_client->sock = sock;
  new_client->next = clients->root;
  clients->root = new_client;
  clients->num_conns++;

  return 0;
}

/*
 * del_client
 *
 * Takes a list of connections and a socket that
 * has disconnected and removes it from the list.
 * FIXME: This may need a lock?
 */
int del_client( int sock )
{
  client_conn *walk, *prev;
  walk = clients->root;
  prev = clients->root;
  
  while( walk )
  {
    if( walk->sock == sock )
    {
      prev->next = walk->next;
      free( walk );
      clients->num_conns--;
      return 0;
    }
    
    walk = walk->next;
  }

  return 1;
}

/*
 * broadcast
 *
 * Takes the list of connections, a message and a message length
 * and broadcasts it to all the clients.
 */
int broadcast( int source_sock, char *msg, int msg_len )
{

  client_conn *walk = clients->root;
  while( walk )
  {
    if( walk->sock != source_sock )
    {
      fprintf(stderr, "sending to %d\n", walk->sock);
      send( walk->sock, msg, msg_len, 0 );
    }
    walk = walk->next;
  }
  return 0;
}

void death_handler( int sig )
{
  client_conn *walk, *to_del;
  char *death_msg = "Server killed, going down!\n";
  broadcast( 0, death_msg, strlen(death_msg) );
  walk = clients->root;
  to_del = clients->root;
  while( walk )
  {
    to_del = walk;
    close( walk->sock );
    walk = walk->next;
    free( to_del );
  }
  free( clients );
  close( sock );
  fprintf( log_file, "%s", death_msg );
  fclose( log_file );
  exit(0);
}
