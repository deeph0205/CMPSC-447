#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "cmpsc447-user.h"

system_t users;
FILE *ifile;
FILE *ofile;
user_t *global_user;
char *global_question_index;
int flag=0;
int num_question;

	       
/******************************************************************************

Project 1 Commands:

user <user_index> <name> 
  Add user of <user_index> and <name> to set of users in the system_t 
  data structure "users" (global) field "members" and update count in
  "userct".  Users cannot be deleted once added.

add <user_index> <question_index> <question_type> <question> <answer(s)>

  Add a new question (<question_type> <question> <answer(s)>) of
  <question_index> for user of <user_index> and add to the "questions"
  field in the user data structure and update the count field "qct".
  Allocate a new question of appropriate data type (<question_type>,
  either int_q or string_q) and initialize all the fields in the
  question data structure.

remove <user_index> <question_index>

  Remove question of <question_index> for user of <user_index>.
  Remove the question from the user data structure ("questions") and
  decrement the count.  Do not worry about leaving any open spaces in
  the questions array.  You must deallocate the question data
  structure. 

change <user_index> <question_index> <question> <answer(s)>
 
  Change question ( <question> <answer(s)>) for <question_index> for
  user of <user_index>.  Replace the "question" and "answer" fields in
  the question data structure.  We cannot change the "question type".

partner <user_index> <question_index_1> <question_index_2>

  Link <question_index_1> and <question_index_2> for authentication
  for user of <user_index> using the field "partner".  Both questions
  must be updated to reflect their partnership.  A question can have
  only one partner, so must update the old partner if a change is
  made.  NOTE: Both questions must be answered if one is chosen at
  login time.

login <user_index> <question_index>

  See if user of <user_index> can answer authentication question
  <question_index> correctly.  If the question chosen has a partner,
  then this question and the partner question (i.e., two questions)
  must be answered successfully to authenticate.

*******************************************************************************/





/******************************************************************************

Task 1: create a new user

Function: system_user_new
Input Args: 
          user_index_str - string for the user index value for the new user
          name - name string for the user
Returns: a new user object or NULL

system_user_new creates a new user of index <user_index> and name <name>.

NOTE: assumes there is no user with this index already

*******************************************************************************/

user_t *system_user_new( char *user_index_str, char *name )
{
  if (users.userct == MAX_USERS) {
    return (user_t *) NULL;
  }
  int user_index = atoi(user_index_str);
  if (users.members[user_index-1]!=(user_t *)NULL){
    return (user_t *) NULL; 
  }
  user_t *new_user = (user_t *) malloc(sizeof(user_t));
  for(int i=0;i<MAX_QUESTIONS;i++){
    new_user->questions[i]=(question_t *)NULL;
  }
  users.members[user_index-1] = new_user;
  users.userct++;
  new_user->id = user_index;
  memcpy(new_user->name, name, MAX_STRING);
  new_user->add_q=user_add_q;
  new_user->change_q=user_change_q;
  new_user->remove_q=user_remove_q;
  new_user->link_q=user_link_q;
  new_user->login=user_login;
  new_user->qct=0;
  // printf("id=%d\n",new_user->id);
  // printf("name=%s\n",new_user->name);
  return new_user;
}


/******************************************************************************

High-level Command Processing Functions: provided

*******************************************************************************/

/******************************************************************************

Function: find_command
Input Args: 
          cmdbuf - buffer for the command line from the input file
          len - length of the cmdbuf
Returns: a command index or an error value (<0)
         
find_command determines the specific command and checks the argument
count.

*******************************************************************************/


int find_command( char *cmdbuf, int len )
{
  int ct =  strct( cmdbuf, len );

  switch( cmdbuf[0] ) {
  case 'a':   // add
    if ( ct != 6 ) {
      return -1;
    }
    return USER_ADD;
    break;
  case 'r':   // remove
    if ( ct != 3 ) {
      return -1;
    }
    return USER_REMOVE;
    break;
  case 'c':   // change
    if ( ct != 5 ) {
      return -1;
    }
    return USER_CHANGE;
    break;
  case 'p':   // partner
    if ( ct != 4 ) {
      return -1;
    }
    return USER_LINK;
    break;
  case 'l':   // login
    if ( ct != 3 ) {
      return -1;
    }
    return USER_LOGIN;
    break;
  case 'u':   // new user 
    if ( ct != 3 ) {
      return -1;
    }
    return USER_NEW;
    break;
  case '#':   // comment lines
  case '%':
  case '\n':
    return USER_SKIP;
    break;
  }

  return -1;
}


/******************************************************************************

Function: find_user
Input Args: 
          user_index_str - the index as a string
          cmd - command number
Returns: a user reference or NULL
         
find_user find the user correpsonding to the index value from 
<user_index_str>, if one exists.

*******************************************************************************/

user_t *find_user( char *user_index_str, int cmd )
{
  int user_index = atoi( user_index_str );  // attack
  int i;

  if ( user_index > 0 ) {
    for ( i = 0; i < users.userct; i++ ) {
      user_t *test_user = users.members[i];
      if ( test_user->id == user_index ) {
	if ( cmd == USER_NEW ) {
	  return (user_t *)NULL;
	}
	return test_user;
      }
    }
  }

  return (user_t *) NULL;
}


/******************************************************************************

Function: apply_command
Input Args: 
          user - user object (may be NULL for USER_NEW)
          cmd - command number
          args - arguments string for the command
Returns: 0 on success and <0 on error
         
Run the respective command on the user object with the exception of
USER_NEW, which creates a user object.

*******************************************************************************/

int apply_command( user_t *user, int cmd, char *args )
{
  printf("calling apply_command\n");
  char arg1[MAX_STRING], arg2[MAX_STRING], arg3[MAX_STRING], arg4[MAX_STRING];
  int res;
  
  switch( cmd ) {
  case USER_NEW:
    if (( sscanf( args, "%s", arg1 )) != 1 ) {
	return -1;
    }
    res = (( user != NULL ) ? 0 : 1 );
    fprintf( ofile, "USER_NEW %s with index %d : result %d\n", arg1, user->id, res );
    return res;
    break;
  case USER_ADD:
    if (( sscanf( args, "%s %s %s %s", arg1, arg2, arg3, arg4 )) != 4 ) {
	return -1;
    }
    res = user->add_q( user, arg1, arg2, arg3, arg4 );
    fprintf( ofile, "USER_ADD %s(%d) for question %s %s %s %s : result %d\n", user->name, user->id, arg1, arg2, arg3, arg4, res );
    return res;
    break;
  case USER_REMOVE:
    if (( sscanf( args, "%s", arg1 )) != 1 ) {
	return -1;
    }
    res = user->remove_q( user, arg1 );
    fprintf( ofile, "USER_REMOVE %s(%d) question of index %s : result %d\n", user->name, user->id, arg1, res );
    return res;
    break;
  case USER_CHANGE:
    if (( sscanf( args, "%s %s %s", arg1, arg2, arg3 )) != 3 ) {
	return -1;
    }
    res = user->change_q( user, arg1, arg2, arg3 );
    fprintf( ofile, "USER_CHANGE %s(%d) for question %s %s %s : result %d\n", user->name, user->id, arg1, arg2, arg3, res );
    return res;
    break;
  case USER_LINK:
    if (( sscanf( args, "%s %s", arg1, arg2 )) != 2 ) {
	return -1;
    }
    res = user->link_q( user, arg1, arg2 );
    fprintf( ofile, "USER_LINK %s(%d) question #%s to question #%s : result %d\n", user->name, user->id, arg1, arg2, res );
    return res;
    break;
  case USER_LOGIN:
    if (( sscanf( args, "%s", arg1 )) != 1 ) {
	return -1;
    }
    res = user->login( user, arg1 );
    fprintf( ofile, "USER_LOGIN for user %s(%d) with question %s gives login result: %s\n",
	     user->name, user->id, arg1, ( res ? "Failed" : "Success" ) );
    return res;
    break;
  default:
    fprintf( ofile, "Skip: apply_command - Invalid command: %d\n", cmd );
    return -1;
    break;
  }
}


  
/******************************************************************************
  
  Task 2: implement user commands (see interface below)

User Command Interface (from cmpsc447-user.h):

extern int user_add_q( user_t *user, char *question_index, char *question_type, char *question, char *answer );
extern int user_remove_q( user_t *user, char *question_index );
extern int user_change_q( user_t *user, char *question_index, char *question, char *answer );
extern int user_link_q( user_t *user, char *question_index, char *other_question_index );
extern int user_login( user_t *user, char *question_index );

Note that these functions need to be assigned to the corresponding
function pointer fields in the user_t data structure

  int (*add_q) ( struct user_type *user, char *question_index, char *question_type,
   	            char *question, char *answer );
  int (*remove_q) ( struct user_type *user, char *question_index );
  int (*change_q) ( struct user_type *user, char *question_index, char *question, char *answer );
  int (*link_q) ( struct user_type *user, char *question_index, char *other_question_index );
  int (*login) ( struct user_type *user, char *question_index );

These functions should implement: (1) the actions that update the
user_t data structure (if needed); (2) find a reference to the
appropriate question to update for the user (from the
<question_index>); (3) determine the question type (int_q or string_q)
from the field qtype in the question; (4) create a reference to the
specific question type; and (5) invoke the appropriate (corresponding)
function pointer for the question type to perform any updates on the
question.

******************************************************************************/
int user_add_q( user_t *user, char *question_index, char *question_type, char *question, char *answer ){
    // printf("hey\n");
    // printf("name=%s\n",user->name);
    // printf("id=%d\n",user->id);
    // printf("type=%s\n",question_type);
    if (user->qct==MAX_QUESTIONS){
      return -1;
    }
    int question_index_int = atoi(question_index);
    if (user->questions[question_index_int-1]!=(question_t *)NULL){
      return -1;
    }
    user->qct+=1;
    if (strcmp(question_type,"string")==0){
        string_q *question_struct= (string_q *)malloc(sizeof(string_q));
        user->questions[question_index_int-1]=(question_t *)question_struct;
        question_struct->add=stringq_add;
        question_struct->remove=question_remove;
        question_struct->change=stringq_change;
        question_struct->link=question_link;
        question_struct->login=stringq_login;
        
        question_struct->type=0;
        question_struct->add(question_struct,question_index_int,question,answer);
    }
    else{
      
        int_q *question_struct= (int_q *)malloc(sizeof(int_q));
        user->questions[question_index_int-1]=(question_t *)question_struct;
        question_struct->add=intq_add;
        question_struct->remove=question_remove;
        question_struct->change=intq_change;
        question_struct->link=question_link;
        question_struct->login=intq_login;
    
        int answer_int=atoi(answer);
        
        question_struct->type=1;
        question_struct->add(question_struct,question_index_int,question,answer_int);
    }
    return 0;
}
int user_remove_q( user_t *user, char *question_index ){
    int question_index_int=atoi(question_index);
    if (user->questions[question_index_int-1]==(question_t *)NULL){
      return -1;
    }
    question_t *question=user->questions[question_index_int-1];
    if (question->partner!=(question_t *)NULL){
      (question->partner)->partner=(question_t *)NULL;
    }
    free(user->questions[question_index_int-1]);
    user->questions[question_index_int-1]=(question_t *)NULL;
    user->qct-=1;
    return 0;

}
int user_change_q( user_t *user, char *question_index, char *question, char *answer ){
    int question_index_int=atoi(question_index);
    if (user->questions[question_index_int-1]==(question_t *)NULL){
      return -1;
    }
    if (user->questions[question_index_int-1]->type==0){
        string_q *question_string=(string_q *)user->questions[question_index_int-1];
        question_string->change(question_string,question,answer);

    }
    else{
        int_q *question_int=(int_q *)user->questions[question_index_int-1];
        int answer_int=atoi(answer);
        question_int->change(question_int,question,answer_int);
    }
    return 0;
}
int user_link_q( user_t *user, char *question_index, char *other_question_index ){
    int question_index_int=atoi(question_index);
    int other_question_index_int=atoi(other_question_index);
    if(user->questions[question_index_int-1]==(question_t *)NULL || user->questions[other_question_index_int-1]==(question_t *)NULL){
      return -1;
    }
    question_t *question=user->questions[question_index_int-1];
    question_t *other_question=user->questions[other_question_index_int-1];
    if (question->partner!=(question_t *)NULL){
      (question->partner)->partner=(question_t *)NULL;
    }
    if (other_question->partner!=(question_t *)NULL){
      (other_question->partner)->partner=(question_t *)NULL;
    }
    question->partner=other_question;
    other_question->partner=question;
    return 0;

}
int user_login( user_t *user, char *question_index ){
    // int question_index_int=atoi(question_index);
    // if (user->questions[question_index_int-1]==(question_t *)NULL){
    //   return -1;
    // }
    // question_t *question=user->questions[question_index_int-1];
    // if (question->partner==(question_t *)NULL){
        
    // }
    // else{
      

    // }
}





/******************************************************************************

  Task 3: Implement question interface

Question Command Interface (from cmpsc447-user.h):

extern int question_remove( question_t *q );
extern int question_link( question_t *q, question_t *other );
extern int intq_add( int_q *iq, int qindex, char *question, int answer );
extern int intq_change( int_q *iq, char *question, int answer );
extern int intq_login( int_q *iq );
extern int stringq_add( string_q *sq, int qindex, char *question, char *answer );
extern int stringq_change( string_q *sq, char *question, char *answer );
extern int stringq_login( string_q *sq );

Note that these functions need to be assigned to the corresponding
function pointer fields in the int_q and string_q data structures.

   (1) Both int_q and string_q:

   extern int question_remove( question_t *q );
   extern int question_link( question_t *q, question_t *other );

       are assigned to

   string_q:
   int (*remove) ( question_t *sq );
   int (*link) ( question_t *sq, question_t *other ); 
 
   int: 
   int (*remove) ( question_t *iq );
   int (*link) ( question_t *iq, question_t *other );  

   (2) For int_q only:

   extern int intq_add( int_q *iq, int qindex, char *question, int answer );
   extern int intq_change( int_q *iq, char *question, int answer );
   extern int intq_login( int_q *iq );

       are assigned to 

   int (*add) ( struct int_question *iq, int qtype, char *question, int answer );
   int (*change) ( struct int_question *iq, char *question, int answer );
   int (*login) ( struct int_question *iq );

   (3) for string_q only:

   extern int stringq_add( string_q *sq, int qindex, char *question, char *answer );
   extern int stringq_change( string_q *sq, char *question, char *answer );
   extern int stringq_login( string_q *sq );

       are assigned to 

   int (*add) ( struct string_question *sq, int qtype, char *question, char *answer );
   int (*change) ( struct string_question *sq, char *question, char *answer );
   int (*login) ( struct string_question *sq );

******************************************************************************/
int question_remove( question_t *q ){

}
int question_link( question_t *q, question_t *other ){

}
int intq_add( int_q *iq, int qindex, char *question, int answer ){
    iq->index=qindex;
    strcpy(iq->question,question);
    iq->answer=answer;
    iq->partner=(question_t *)NULL;
    return 0;
    // printf("type=%d\n",iq->type);
    // printf("index=%d\n",iq->index);
    // printf("questiom=%s\n",iq->question);
    // printf("answer=%d\n",iq->answer);

}
int intq_change( int_q *iq, char *question, int answer ){

    strcpy(iq->question,question);
    iq->answer=answer;
    return 0;
}
int intq_login( int_q *iq ){

}
int stringq_add( string_q *sq, int qindex, char *question, char *answer ){
    sq->index=qindex;
    strcpy(sq->question,question);
    strcpy(sq->answer,answer);
    sq->partner=(question_t *)NULL;
    return 0;
    // printf("type=%d\n",sq->type);
    // printf("index=%d\n",sq->index);
    // printf("questiom=%s\n",sq->question);
    // printf("answer=%s\n",sq->answer);
}
int stringq_change( string_q *sq, char *question, char *answer ){
    strcpy(sq->question,question);
    strcpy(sq->answer,answer);
    return 0;
}
int stringq_login( string_q *sq ){

}



/******************************************************************************

Task 4: command processing from input file

Function: main

Input Args: 
          argc - The number of command line arguments (3)
          argv - Command line arguments ( cmpsc447-p1 <input_file> <output_file>
Returns: exit status

Main runs the sequence of commands listed in <input_file> and stores the 
results of the commands in <output_file>.  The output fprintf statements
are provided.

NOTE: Need to complete three fragments of this function to: (1) get input from
the input file; (2) compute a reference to the <user_index> string for find_user;
and (3) compute a reference to the arguments list for apply_command.

Complete the "TBD(x)" functionality below for these three cases.

*******************************************************************************/

int main( int argc, char *argv[] )
{
  for(int i=0;i<MAX_USERS;i++){
    users.members[i]=(user_t *)NULL;
  }

  user_t *user;
  FILE *file;
  char *line = (char *)NULL;
  int cmd;
  char *cmdstr, *args;
  int res;
  int len = MAX_LINE;
  char user_index_str[MAX_STRING];
	
  // check usage
  if ( argc != 3 ) {
    printf( "Main: Err: Incorrect usage: %s\n", USAGE );
  }

  // open fds
  if (( file = fopen( argv[1], "r" )) == NULL ) {
    exit(-1);
  }

  ifile = file;

  if (( file = fopen( argv[2], "w+" )) == NULL ) {
    exit(-1);
  }

  ofile = file;
	 
  // get input - line-by-line from the input file
  // get input - line-by-line from the input file

  line = (char *) malloc(MAX_LINE);
  while (1) {  // TBD (1): get a line of input from the input file

    bzero(line, MAX_LINE);
    char* status = fgets(line, len, ifile);
    if (status == NULL) {
      break;
    }

    cmdstr = line;

    // etermine command from command string (first arg in line)

    cmd = find_command( cmdstr, strlen( cmdstr ));

    if ( cmd == USER_SKIP ) {
      continue;
    }
    else if ( cmd < 0 ) {
      fprintf( ofile, "ERR: invalid command line: %s", line );
      continue;
    }
	  
    // TBD (2): copy <user_index> from cmdstr into user_index_str


    // get the user object
    

    int first_space = 0;
    char space = ' ';
    while (cmdstr[first_space] != space) {
      first_space++;
    }
    first_space++;

    int second_space = first_space;
    while (cmdstr[second_space] != space) {
      second_space++;
    }

    memcpy(user_index_str, cmdstr + first_space, second_space - first_space);

    


    user = find_user( user_index_str, cmd );  
    if (( user == NULL ) && ( cmd != USER_NEW )) {
      fprintf( ofile, "ERR: Unknown user: %s", line );
      continue;
    }

    // TBD (3): get reference to args from cmdstr

    // apply the command
    // user new is special
    args = cmdstr + second_space + 1;

    if ( cmd == USER_NEW ) {
      user = system_user_new( user_index_str, args );
    }
    
    res = apply_command( user, cmd, args );
	  
    if ( res < 0 ) {
      fprintf( ofile, "ERR: failed command: cmd %d for %s(%d) for line %s\n", cmd, user->name, user->id, line );
      continue;
    }
    // break;
  }
  free(line);
  exit( 0 );
}


