/*
 * tirc.h
 * 
 * Main header file for the tirc program.
 *
 * Author: Jeffrey Picard
 */
#ifndef _TIRC_H
#define _TIRC_H

#include <stdio.h>
/* Globals */
FILE *log_file;

#define EXIT_WITH_ERROR( ... ) do { \
  fprintf( log_file, __VA_ARGS__ );   \
  exit(-1);                         \
} while(0)

#define NULL_CHECK( p, ... ) do {   \
  if( !p )                          \
    EXIT_WITH_ERROR( __VA_ARGS__ ); \
} while(0)

#define NEG_CHECK( p, ... ) do {    \
  if( p < 0 )                       \
    EXIT_WITH_ERROR( __VA_ARGS__ ); \
} while(0)

struct _client_conn {
  int sock;
  struct _client_conn *next;
} typedef client_conn;

struct _conn_list {
  int num_conns;
  client_conn *root;
} typedef conn_list;

struct _serv_args {
  int sock_client;
  conn_list *clients;
} typedef serv_args;

#endif
