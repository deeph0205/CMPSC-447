#define USER_ADD 0
#define USER_REMOVE 1
#define USER_CHANGE 2
#define USER_LINK 4
#define USER_LOGIN 5
#define USER_NEW 6
#define USER_SKIP 7

#define INT_QUESTION 0
#define STRING_QUESTION 1

#define MAX_USERS 100
#define MAX_QUESTIONS 10
#define MAX_STRING 30
#define MAX_LINE 100
#define QNEEDED 1

#define USAGE "cmpsc447-p1 cmd-file output-file"


typedef struct question {
	int index;
        int type;
        struct question *partner;
} question_t;

typedef struct int_question {
	int index;
        int type;
	question_t *partner;
        char question[MAX_STRING];
	int answer;
        int (*add) ( struct int_question *iq, int qtype, char *question, int answer );
        int (*change) ( struct int_question *iq, char *question, int answer );
        int (*remove) ( question_t *iq );
        int (*link) ( question_t *iq, question_t *other );  
        int (*login) ( struct int_question *iq );
} int_q; 

typedef struct string_question {
	int index;
        int type;
	question_t *partner;
        char question[MAX_STRING];
	char answer[MAX_STRING];
        int (*add) ( struct string_question *sq, int qtype, char *question, char *answer );
        int (*change) ( struct string_question *sq, char *question, char *answer );
        int (*remove) ( question_t *sq );
        int (*link) ( question_t *sq, question_t *other ); 
        int (*login) ( struct string_question *sq );
} string_q; 

typedef struct user_type {
	int id;
        int qct;
	char name[MAX_STRING];
	question_t *questions[MAX_QUESTIONS];
        int (*add_q) ( struct user_type *user, char *question_index, char *question_type,
		       char *question, char *answer );
        int (*remove_q) ( struct user_type *user, char *question_index );
        int (*change_q) ( struct user_type *user, char *question_index, char *question, char *answer );
        int (*link_q) ( struct user_type *user, char *question_index, char *other_question_index );
        int (*login) ( struct user_type *user, char *question_index );
} user_t;

typedef struct system {
        int userct;
        user_t *members[MAX_USERS];
} system_t;


#define SPACE_CHAR( str,i ) \
	(( (str)[(i)] == ' ' ) || ( (str)[(i)] == '\t' ) || ( (str)[(i)] == '\n' )) 


static inline int strct( char *str, int len )
{
  int ct = 0;
  int instr = 0;
  int i;

  for ( i = 0; i < len; i++ ) {
    if (( !SPACE_CHAR( str, i )) && instr );
    else if (( !SPACE_CHAR( str, i )) && !instr ) { instr = 1, ct++; }
    else if ( SPACE_CHAR( str, i )) { instr = 0; }
    else { return -1; }
  }

  return ct;
}


/********************

externs for function pointer implementations 

*********************/

// system
extern user_t *system_user_new( char *user_index_str, char *name );


// user
extern int user_add_q( user_t *user, char *question_index, char *question_type, char *question, char *answer );
extern int user_remove_q( user_t *user, char *question_index );
extern int user_change_q( user_t *user, char *question_index, char *question, char *answer );
extern int user_link_q( user_t *user, char *question_index, char *other_question_index );
extern int user_login( user_t *user, char *question_index );

// questions
extern int question_remove( question_t *q );
extern int question_link( question_t *q, question_t *other );
extern int intq_add( int_q *iq, int qindex, char *question, int answer );
extern int intq_change( int_q *iq, char *question, int answer );
extern int intq_login( int_q *iq );
extern int stringq_add( string_q *sq, int qindex, char *question, char *answer );
extern int stringq_change( string_q *sq, char *question, char *answer );
extern int stringq_login( string_q *sq );
