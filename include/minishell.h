#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft_updated/libft.h"
# include <stdio.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/types.h>
# include <errno.h>
# include <sys/wait.h>
# include <signal.h>

# define PROMPT "shit-shell: " //"💩-shell: "

//Global variable for signal handling

/*

volatile:
Tells the compiler not to optimize access to this variable
Prevents the compiler from caching the variable's value in registers
Important for variables that can be modified by external events (like signal handlers)
Without volatile, the compiler might assume the value never changes and optimize incorrectly

WITHOUT volatile:
while (signal_flag)
{
    Compiler might optimize this to an infinite loop
    because it assumes signal_flag never changes
}

WITH volatile:
while (volatile_signal_flag)
{
    Compiler will check the value each time
    because it knows it might change externally
}

sig_atomic_t:
Special integer type guaranteed to be read/written atomically
"Atomic" means the operation can't be interrupted halfway
Prevents race conditions in signal handlers
Usually a small integer type that can be accessed in one CPU instruction
Perfect for flags and signal numbers

Together volatile sig_atomic_t:

volatile: Ensures the value is always read from memory
sig_atomic_t: Ensures the read/write is atomic

*/

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,     //>
	TOKEN_REDIRECT_IN,      //<
	TOKEN_REDIRECT_APPEND,  //>>
	TOKEN_HEREDOC,          //<<
}   t_token_type;

typedef	enum e_quote_state
{
	QUOTE_NONE,
	QUOTE_SINGLE,
	QUOTE_DOUBLE
}	t_quote_state;


typedef struct s_quote_context
{
	char				*buffer;
	size_t				capacity;
	size_t				length;
	char				quote_type;
	t_quote_state		state;
}   t_quote_context;

typedef struct s_token
{
	t_token_type		type;		// Type of token (e.g., WORD, PIPE)
	char				*content;
	t_quote_state		quote_state;
	struct s_token		*next;		// Pointer to the next token
	struct s_mshell		*mshell;	// Pointer to the mshell object
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
	int			exec_cmds;     		// Total number of executable commands
	int			allocated_pipes;	// Number of pipes allocated
	int			**pipfd;        	// File descriptors for pipes
	t_ast_node	*ast;
	t_token		*token;
	int			exit_code;
	int			pipes_count;
	pid_t		*pids;
	int			cur_pid;
	char 		**envp;
	int			fd_in;
	int			fd_out;
	int			interactive_mode;  // Flag for interactive mode
}	t_mshell;

int				error_ret(int type, char *arg);
void			clean_mshell(t_mshell *obj);
void			close_fds(t_mshell *obj);

/* ===== PARSING UTILS (parsing_utils.c) ===== */
int				ft_isspace(int c);
int				is_operator(char c);
int				is_word_char(char c);
int				is_quote(char c);
t_quote_state	get_quote_state(char quote);

/* ===== PARSING ===== */
void			parse(t_mshell *obj);
void			print_parse_debug(t_mshell *obj);
t_token			*tokenize(const char *input, t_mshell *mshell);
void			init_tokenize(t_token **head, t_token **current);
t_token			*process_trimmed_input(t_token **head, t_token **current, char *trimmed, t_mshell *mshell);
char			**fetch_paths(char **envp, int first);

/* ===== TOKEN CORE (token_core.c) ===== */
t_token 		*new_token(t_token_type type, const char *content, size_t len, t_mshell *mshell);
void			link_token(t_token **head, t_token **current, t_token *new);
void			clean_tokens(t_token *head);
//t_token			*process_token(t_token **head, t_token **current, const char *input, int *i);

/* ===== ENVIRONMENT VARIABLES (token_env.c) ===== */
char			*get_env_value(const char *var_name, t_mshell *mshell);
char			*expand_env_vars(const char *str, t_mshell *mshell);

/* ===== TOKEN PROCESS (token_process.c) ===== */
t_token			*handle_operator(t_token **head, t_token **current, const char *input, int *i);
t_token			*handle_word(t_token **head, t_token **current, const char *input, int *i);
t_token			*process_token(t_token **head, t_token **current, const char *input, int *i);

/* ===== QUOTE HANDLING (token_quote.c) ===== */
t_token			*handle_single_quotes(const char *input, int *i, t_mshell *mshell);
t_token			*handle_double_quotes(const char *input, int *i, t_mshell *mshell);
t_token			*handle_quotes(t_token **head, t_token **current, const char *input, int *i);

/* ===== BUILT-INS ===== */
int			open_dir(const char *dir);
void		pwd(void);
void		set_env_args(t_mshell *obj, t_ast_node *node);
int			env(void);
void		echo(char **args);
int			export(char **args);
int			unset(char **args, t_mshell *obj);

/* ===== AST CORE (ast_core.c) ===== */
int				is_redirect_token(t_token_type type);
t_ast_node		*create_ast_node(t_token_type type);
t_ast_node		*parse_pipeline(t_token **tokens, int i, t_mshell *obj);
t_ast_node		*parse_command(t_token **tokens);
void			free_ast(t_ast_node *node);
t_ast_node		*free_ast_return_null(t_ast_node *node);

/* ===== AST COMMAND (ast_command.c) ===== */
t_ast_node		*build_command_node(t_token **tokens);

/* ===== AST DEBUG (ast_debug.c) ===== */
void			print_ast(t_ast_node *node, int depth);
void			print_tokens(t_token *tokens);


/* ===== EXECUTION ===== */
void	print_exit(char *mes, char *cmd, int exit_code);
char	*check_paths_access(char **paths, t_ast_node *node, t_mshell *obj);
void	execute_cmd(t_mshell *obj, t_ast_node *left, t_ast_node *right);
char	**read_alloc(int fd, size_t *i);
void	choose_actions(t_mshell *obj);
void	exit_child(t_mshell *obj, char *arg, int exit_code);

/* ===== REDIRECTION ===== */
void	redirection_input(t_mshell *obj, t_ast_node *node);
void	redirection_output(t_mshell *obj, t_ast_node *node);
void	pipe_redirection(t_mshell *obj);
void	handle_here_doc(t_mshell *obj, t_ast_node *node);

/* ===== CLEANUP ===== */
void	clean_strs(char **strs);


/* ===== SIGNALS ===== */
void	setup_shell_signals(t_mshell *mshell);
int		init_shell_mode(t_mshell *mshell);
void	reset_signals_to_default(void);
void	setup_execution_signals(void);
void	handle_sigint(int sigint);
void	handle_sigquit(int sig);
void	rl_replace_line(const char *text, int clear_undo);
void	re_on_new_line(void);
void	rl_redisplay(void);


#endif
