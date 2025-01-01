#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft_updated/libft.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>

# define PROMPT "💩-shell: "

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,     //>
	TOKEN_REDIRECT_IN,      //<
	TOKEN_REDIRECT_APPEND,  //>>
	TOKEN_HEREDOC,          //<<
	TOKEN_ENV_VAR,			//for $VAR
	TOKEN_EXIT_STATUS		//for $?
}   t_token_type;

typedef	enum e_quote_state
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_state;

typedef struct s_env_data
{
	const char	*input;
	int			start;
	int			len;
}	t_env_data;

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
	int			last_exit_status;
	int			pipes_count;
}	t_mshell;

int				error_ret(int type, char *arg);
void			clean_mshell(t_mshell *obj);

/* ===== PARSING UTILS (parsing_utils.c) ===== */
int				ft_isspace(int c);
int				is_operator(char c);
int				is_word_char(char c);
int				is_quote(char c);
t_quote_state	get_quote_state(char quote);

/* ===== PARSING ===== */
void			parse(t_mshell *obj);
void			print_parse_debug(t_mshell *obj);
t_token			*tokenize(const char *input);
void			init_tokenize(t_token **head, t_token **current);
t_token			*process_trimmed_input(t_token **head, t_token **current, char *trimmed);
char			**fetch_paths(char **envp);

/* ===== TOKEN CREATION (token_create.c) ===== */
t_token			*new_token(t_token_type type, const char *start, int length);
char			*allocate_token_content(const char *start, int length);
t_token			*create_token_from_str(const char *str, t_token_type type, int len);
void			link_token(t_token **head, t_token **current, t_token *new);
t_token			*process_token(t_token **head, t_token **current, const char *input, int *i);
int				extend_current_token(t_token **current, const char *input, int *i);

/* ===== WORD TOKENS (token_word.c) ===== */
void			add_word_token(t_token **head, t_token **current, const char *input, int *i);
t_token			*create_word_token(const char *input, int start, int len);
int				calculate_word_length(const char *input, int *i);
char			**fetch_paths(char **envp);
t_quote_state	get_quote_state(char quote);

/* ===== OPERATOR TOKENS (token_operator.c) ===== */
void			add_operator_token(t_token **head, t_token **current, const char *input, int *i);
t_token			*handle_double_redirect(const char *input, int *i);
t_token			*handle_single_operator(const char *input, int *i);

/* ===== QUOTE HANDLING (token_quote.c) ===== */
void			add_quoted_token(t_token **head, t_token **current, const char *input, int *i);
t_token			*handle_quote_content(const char *input, int *i, char quote);
t_token			*handle_double_quote_content(const char *input, int *i, int start);
int				find_closing_quote(const char *input, int *i, char quote_char);
t_token			*handle_double_quote_env(const char *input, int *i, int start);

/* ===== ENVIRONMENT VARIABLES (token_env.c) ===== */
void			handle_env_var(t_token **head, t_token **current, const char *input, int *i);
t_token			*handle_env_var_token(const char *input, int *i);
void			add_exit_status_token(t_token **head, t_token **current);
int				get_env_var_len(const char *str);

/* ===== CLEANUP (token_clean.c) ===== */
void			clean_token(t_token *token);
void			clean_token_list(t_token *head);
void			clean_parse_error(t_token **head, t_token **current);

/* ===== BUILT-INS ===== */
void			open_dir(const char *dir);
void			pwd(void);
void			env(char **env_args, char **envp);
void			echo(char **args);

/* ===== AST CORE (ast_core.c) ===== */
t_ast_node		*create_ast_node(t_token_type type);
t_ast_node		*parse_pipeline(t_token **tokens);
t_ast_node		*parse_command(t_token **tokens);
void			free_ast(t_ast_node *node);

/* ===== AST COMMAND (ast_command.c) ===== */
t_ast_node		*build_command_node(t_token **tokens);

/* ===== AST REDIRECT (ast_redirect.c) ===== */
t_ast_node		*handle_redirection(t_token **tokens, t_ast_node *cmd_node);

/* ===== AST DEBUG (ast_debug.c) ===== */
void			print_ast(t_ast_node *node, int depth);


#endif
