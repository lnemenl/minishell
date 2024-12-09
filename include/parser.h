// parser.h
#ifndef PARSER_H
# define PARSER_H


typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_OUT,     //>
    TOKEN_REDIRECT_IN,      //<
    TOKEN_REDIRECT_APPEND,  //>>
    TOKEN_HEREDOC           //<<
} t_token_type;

typedef struct s_token
{
    t_token_type      type;
    char              *value;
    struct s_token    *next;
} t_token;

typedef struct s_redirection
{
    int                   type; // e.g. REDIR_OUT, REDIR_IN, etc.
    char                  *filename;
    struct s_redirection  *next;
} t_redirection;

typedef struct s_command
{
    char           **arguments; // NULL-terminated array of strings
    t_redirection  *redir;      // Linked list of redirections
    struct s_command *next;     // Next command in pipeline
} t_command;

#endif
