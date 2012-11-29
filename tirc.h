/*
 * tirc.h
 *
 * Main header file for the tirc program.
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
