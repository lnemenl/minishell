/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 12:26:56 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/18 13:21:43 by rkhakimu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include <termios.h>
# include <sys/ioctl.h>

# ifndef PATH_BUFFER_SIZE
#  define PATH_BUFFER_SIZE 4096
# endif

# define PROMPT "shit-shell: "

extern volatile sig_atomic_t g_signal_received;


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

typedef enum e_signal_state
{
    SIGNAL_STATE_INTERACTIVE,
    SIGNAL_STATE_HEREDOC,
    SIGNAL_STATE_RESET
}	t_signal_state;


typedef struct s_quote_context
{
	char			*buffer;
	size_t			capacity;
	size_t			length;
	char			quote_type;
	t_quote_state	state;
}   t_quote_context;

typedef struct s_token
{
	t_token_type	type;		// Type of token (e.g., WORD, PIPE)
	char			*content;
	t_quote_state	quote_state;
	struct s_token	*next;		// Pointer to the next token
	struct s_mshell	*mshell;	// Pointer to the mshell object
	int				is_quote_heredoc;
}   t_token;


typedef struct s_ast_node
{
	t_token_type		type;       // Type of AST node: PIPE, REDIRECTION, or WORD
	char				**args;     // Command arguments (for TOKEN_WORD nodes)
	struct s_ast_node	*left;      //Left child (e.g., command before a pipe)
	struct s_ast_node	*right;     //Right child (e.g., command after a pipe or target of a redirection)
	struct s_ast_node	**redirs;	//for redirections, order is preserved as encountered
	int					is_quote_heredoc;
}   t_ast_node;

typedef struct	s_mshell
{
	char				*cmd_line;      	// Full input line entered by the user
	char				**cmds;         	// Array of command strings (split version of cmd_line)
	char				*cur_path;      	// Current working directory path
	char				**paths;
	int					exec_cmds;     		// Total number of executable commands
	int					allocated_pipes;	// Number of pipes allocated
	int					**pipfd;        	// File descriptors for pipes
	t_ast_node			*ast;
	t_token				*token;
	uint8_t				exit_code;
	int					pipes_count;
	pid_t				*pids;
	int					cur_pid;
	char 				**envp;
	int					fd_in;
	int					fd_out;
	size_t				args_move;
	int					redir_check;
	int					heredoc_interrupted;
	struct s_heredoc	*heredoc;
	int					stdin_fd;
	char				*prev_path;
}	t_mshell;

typedef struct s_heredoc
{
    char		*str;
    char		*trimmed;
    char		*expanded;
	int			pipe_fd[2];
    t_mshell	*obj;
}   t_heredoc;

int						error_ret(int type, char *arg);
void					clean_mshell(t_mshell *obj);
void					close_fds(t_mshell *obj);

/* ===== PARSING		 UTILS (parsing_utils.c) ===== */
int						ft_isspace(int c);
int						is_operator(char c);
int						is_word_char(char c);
int						is_quote(char c);
t_quote_state			get_quote_state(char quote);

/* ===== PARSING		 ===== */
void					parse(t_mshell *obj);
t_token					*tokenize(const char *input, t_mshell *mshell);
void					init_tokenize(t_token **head, t_token **current);
t_token					*process_trimmed_input(t_token **head, t_token **current, char *trimmed, t_mshell *mshell);
char					**fetch_paths(char **envp);

/* ===== TOKEN C		ORE (token_core.c) ===== */
t_token 				*new_token(t_token_type type, const char *content, size_t len, t_mshell *mshell);
void					link_token(t_token **head, t_token **current, t_token *new);
void					clean_tokens(t_token *head);

/* ===== ENVIRON		MENT VARIABLES (token_env.c) ===== */
char					*get_env_value(const char *var_name, t_mshell *mshell);
char					*expand_env_vars(const char *str, t_mshell *mshell);

/* ===== TOKEN P		ROCESS (token_process.c) ===== */
t_token					*handle_operator(t_token **head, t_token **current, const char *input, int *i);
t_token					*handle_word(t_token **head, t_token **current, const char *input, int *i);
t_token					*process_token(t_token **head, t_token **current, const char *input, int *i);

/* ===== QUOTE H		ANDLING (token_quote.c) ===== */
t_token					*handle_single_quotes(const char *input, int *i, t_mshell *mshell);
t_token					*handle_double_quotes(const char *input, int *i, t_mshell *mshell, t_token_type current_type);
t_token					*handle_quotes(t_token **head, t_token **current, const char *input, int *i);
char					*handle_backslash(char *str);

/* ===== BUILT-INS ===== */
int			cd(char **cd_args, t_mshell *obj);
int			pwd(t_mshell *obj);
void		set_env_args(t_mshell *obj, t_ast_node *node);
int			env(t_mshell *obj, char **args);
int			echo(char **args, t_mshell *obj, int is_quote);
int			export(char **args, t_mshell *obj);
int			unset(char **args, t_mshell *obj);
void		check_and_handle_exit(char **args, t_mshell *obj);

/* ===== AST CORE (ast_core.c) ===== */
t_ast_node				*create_ast_node(t_token_type type);
t_ast_node				*free_ast_return_null(t_ast_node *node);
void					free_ast(t_ast_node *node);
int						is_redirect_token(t_token_type type);
t_ast_node				*parse_simple_command(t_token **tokens);
t_ast_node				*parse_command(t_token **tokens);
t_ast_node				*parse_pipeline(t_token **tokens);

/* ===== EXECUTION ===== */
void	print_exit(char *mes, char *cmd, int exit_code);
char	*check_paths_access(char **paths, t_ast_node *node, t_mshell *obj);
void	execute_cmd(t_mshell *obj, t_ast_node *cmd);
char	**read_alloc(int fd, size_t *i);
void	choose_actions(t_mshell *obj);
void	exit_child(t_mshell *obj, char *arg, int exit_code, int is_builtin);
size_t	get_envp_memory_size(char **envp);
size_t	get_envp_length(char **envp);
int		is_env_created(char *arg, char **strs);
char	*get_env_var(char **envp, const char *var_name);

/* =====				 REDIRECTION ===== */
void	redirection_input(t_mshell *obj, t_ast_node *node);
void	redirection_output(t_mshell *obj, t_ast_node *node);
void	pipe_redirection(t_mshell *obj, t_ast_node *cmd);
int		handle_here_doc(t_mshell *obj, t_ast_node *node, int last_fd);

/* =====				 CLEANUP ===== */
void	clean_strs(char **strs);


/* ===== SIGNALS ===== */
void 	setup_interactive_signals(void);
void 	setup_exec_signals(void);
void 	setup_heredoc_signals(void);
void	transition_signal_handlers(t_signal_state new_state);
void	disable_echoctl(void);


#endif