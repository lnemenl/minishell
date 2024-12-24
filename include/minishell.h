#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft_updated/libft.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>

# define PROMPT "mega-shell: "

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,     //>
	TOKEN_REDIRECT_IN,      //<
	TOKEN_REDIRECT_APPEND,  //>>
	TOKEN_HEREDOC           //<<
}   t_token_type;

typedef	enum e_quote_state
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_state;

typedef struct s_token
{
	t_token_type		type;		// Type of token (e.g., WORD, PIPE)
	char				*start;		// Pointer to the start of the token in the input string
	int					length;		// Length of the token
	struct s_token		*next;		// Pointer to the next token
	t_quote_state		quote_state;
}   t_token;


typedef struct s_ast_node
{
	t_token_type		type;       // Type of AST node: PIPE, REDIRECTION, or WORD
	char				**args;     // Command arguments (for TOKEN_WORD nodes)
	struct s_ast_node	*left;      //Left child (e.g., command before a pipe)
	struct s_ast_node	*right;     //Right child (e.g., command after a pipe or target of a redirection)
}   t_ast_node;

typedef struct	s_mshell
{
	char		*cmd_line;      	// Full input line entered by the user
	char		**cmds;         	// Array of command strings (split version of cmd_line)
	char		*cur_path;      	// Current working directory path
	char		**paths;
	int			is_heredoc;     	// Flag for active heredoc mode
	int			total_cmds;     	// Total number of commands
	int			allocated_pipes;	// Number of pipes allocated
	int			**pipfd;        	// File descriptors for pipes
	t_ast_node	*ast;
}	t_mshell;

//parsing

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


//ast
t_ast_node	*create_ast_node(t_token_type type);
t_ast_node	*build_command_node(t_token **tokens);
t_ast_node	*parse_pipeline(t_token **tokens);
t_ast_node	*handle_redirection(t_token **tokens, t_ast_node *cmd_node);
t_ast_node	*parse_command(t_token **tokens);
void		free_ast(t_ast_node *node);

//To be able to see for now whether it all works correctly
void		print_ast(t_ast_node *node, int depth);


#endif
