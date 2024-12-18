#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft_updated/libft.h"
# include <stdio.h>

typedef struct	s_mshell
{
	char	*cmd_line;      // Full input line entered by the user
    char    **cmds;         // Array of command strings (split version of cmd_line)
	char	*cur_path;      // Current working directory path
	char    **paths;
    int		is_heredoc;     // Flag for active heredoc mode
	int		total_cmds;     // Total number of commands
	int		allocated_pipes;// Number of pipes allocated
	int		**pipfd;        // File descriptors for pipes
}	t_mshell;

//parsing

typedef enum e_token_type
{
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_REDIRECT_OUT,     //>
    TOKEN_REDIRECT_IN,      //<
    TOKEN_REDIRECT_APPEND,  //>>
    TOKEN_HEREDOC           //<<
}   t_token_type;

typedef struct s_token
{
    t_token_type      type;   // Type of token (e.g., WORD, PIPE)
    const char        *start; // Pointer to the start of the token in the input string
    int               length; // Length of the token
    struct s_token    *next;  // Pointer to the next token
}   t_token;

typedef struct s_ast_node
{
    t_token_type        type;       // Type of AST node: PIPE, REDIRECTION, or WORD
    char                **args;     // Command arguments (for TOKEN_WORD nodes)
    struct s_ast_node   *next;      // Pointer to the next node in the flat list
}   t_ast_node;

int		error_ret(int type, char *arg);
char	**fetch_paths(char **envp);
void	clean_mshell(t_mshell *obj);

//parsing
void        parse(t_mshell *obj);
t_token     *tokenize(const char *input);
t_token     *new_token(t_token_type type, const char *start, int length);
void        add_operator_token(t_token **head, t_token **current, const char *input, int *i);
void        add_quoted_token(t_token **head, t_token **current, const char *input, int *i);
void        add_word_token(t_token **head, t_token **current, const char *input, int *i);

#endif
