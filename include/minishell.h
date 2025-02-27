/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkhakimu <rkhakimu@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 12:26:56 by msavelie          #+#    #+#             */
/*   Updated: 2025/02/27 16:02:39 by rkhakimu         ###   ########.fr       */
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

extern volatile sig_atomic_t	g_signal_received;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_OUT,
	TOKEN_REDIRECT_IN,
	TOKEN_REDIRECT_APPEND,
	TOKEN_HEREDOC,
}	t_token_type;

typedef enum e_quote_state
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
	char				*buffer;
	size_t				capacity;
	size_t				length;
	char				quote_type;
	t_quote_state		state;
}	t_quote_context;

typedef struct s_token
{
	t_token_type		type;
	char				*content;
	t_quote_state		quote_state;
	struct s_token		*next;
	struct s_mshell		*mshell;
	int					is_quote_heredoc;
}	t_token;

typedef struct s_ast_node
{
	t_token_type		type;
	char				**args;
	struct s_ast_node	*left;
	struct s_ast_node	*right;
	struct s_ast_node	**redirs;
	int					is_quote_heredoc;
}	t_ast_node;

typedef struct s_mshell
{
	char				*cmd_line;
	char				*cur_path;
	char				**paths;
	int					exec_cmds;
	int					allocated_pipes;
	int					**pipfd;
	t_ast_node			*ast;
	t_token				*token;
	uint8_t				exit_code;
	int					pipes_count;
	pid_t				*pids;
	int					cur_pid;
	char				**envp;
	int					fd_in;
	int					fd_out;
	size_t				args_move;
	int					redir_check;
	int					heredoc_interrupted;
	struct s_heredoc	*heredoc;
	char				*prev_path;
	int					*heredoc_fds;
	int					heredocs_count;
	int					current_heredoc;
	char				**exp_args;
	int					parse_success;
}	t_mshell;

typedef struct s_quote_data
{
	const char			*input;
	int					start;
	int					*i;
	t_mshell			*mshell;
	t_token_type		current_type;
}	t_quote_data;

typedef struct s_heredoc
{
	char				*str;
	char				*trimmed;
	char				*expanded;
	int					pipe_fd[2];
	t_mshell			*obj;
}	t_heredoc;

/* ===== PARSING UTILS (parsing_utils.c) ===== */
int				ft_isspace(int c);
int				is_operator(char c);
int				is_word_char(char c);
int				is_quote(char c);
t_quote_state	get_quote_state(char quote);

/* ===== PARSING ===== */
char			**fetch_paths(char **envp);
void			tokenize_and_count_pipes(t_mshell *obj);
void			init_tokenize(t_token **head, t_token **current);
t_token			*tokenize(const char *input, t_mshell *mshell);
void			parse(t_mshell *obj);
void			construct_parse_tree(t_mshell *obj);
t_token			*init_and_process(t_token **head, t_token **current,
					char *trimmed_input, t_mshell *mshell);
t_token			*process_trimmed_input(t_token **head, t_token **current,
					char *trimmed, t_mshell *mshell);

/* ===== ENV FUNCTIONS ===== */
char			**copy_envp(char **envp);
char			*get_var_name(const char *str, int *i);
char			*get_env_value(const char *var_name, t_mshell *mshell);
char			*join_and_free(char *s1, char *s2);
char			*extract_quoted_var_name(const char *str, int *i);
char			*handle_quoted_var(char *buffer, const char *input, int *i);
char			*handle_regular_var(char *buffer,
					const char *input, int *i, t_mshell *mshell);
char			*handle_dollar_expansion(char *buffer,
					const char *input, int *i, t_mshell *mshell);
char			*expand_env_vars(const char *input,
					t_mshell *mshell, int clean);
void			put_env_var(t_mshell *obj, char *new_arg, char *hint);
void			set_env_args(t_mshell *obj, t_ast_node *node);
size_t			get_envp_length(char **envp);
int				is_env_created(char *arg, char **strs, char *hint);
char			*get_env_var(char **envp, const char *var_name);
char			*check_env_arg(char *arg);
char			*remove_quotes(const char *str);
char			*append_until_dollar(char *buffer, const char *input, int *i);
char			*handle_pid_expansion(char *buffer, t_mshell *mshell, int *i);
void			join_put_env(t_mshell *obj, char *name, char *value);
void			set_pwds(t_mshell *obj);
void			create_new_var(t_mshell *obj, char *new_arg, char ***dest);
void			replace_env(t_mshell *obj, char *new_arg, char **dest, int pos);
char			*choose_expand_type(const char *input,
					t_token *current, char *temp);
char			*process_expansion_buffer(char *buffer, int clean);

/* ===== INVALID CASES ===== */
int				is_cmd_line_invalid(t_mshell *obj);
int				is_ast_invalid(t_mshell *obj);
int				is_main_signaled(t_mshell *obj);
int				error_ret(int type, char *arg);

/* ===== TOKEN CORE (token_core.c) ===== */
t_token			*new_token(t_token_type type, const char *content,
					size_t len, t_mshell *mshell);
void			link_token(t_token **head, t_token **current, t_token *new);
void			clean_tokens(t_token *head);

/* ===== TOKEN PROCESS (token_process.c) ===== */
t_token			*handle_operator(t_token **head, t_token **current,
					const char *input, int *i);
t_token			*handle_word(t_token **head, t_token **current,
					const char *input, int *i);
t_token			*process_token(t_token **head, t_token **current,
					const char *input, int *i);
t_token			*handle_quoted_word(t_token *current, char *expanded);
t_token_type	get_operator_type(const char *input, int *i);
char			*extract_word(const char *input, int *i);
char			*process_word(char *temp, t_mshell *mshell, int clean);

/* ===== QUOTE HANDLING (token_quote.c) ===== */
t_token			*handle_single_quotes(const char *input, int *i,
					t_mshell *mshell);
t_token			*handle_double_quotes(const char *input, int *i,
					t_mshell *mshell, t_token_type current_type);
t_token			*handle_quotes(t_token **head, t_token **current,
					const char *input, int *i);
t_token			*process_quoted_content(t_quote_data *data);
t_token			*process_quote_token(t_token **current, const char *input,
					int *i, char quote);
char			*extract_quoted_content(const char *input, int start, int end);
char			*process_content(char *content, t_token_type type,
					t_mshell *mshell);
t_token			*create_quoted_token(char *expanded, const char *input,
					int start, t_mshell *mshell);
t_token			*join_word_token(t_token *prev_token, t_token *token);
t_token			*process_word_token(t_token **head, t_token **current,
					t_token *token, int in_word);

/* ===== BUILT-INS ===== */
int				is_builtin_cmd(char *cmd);
int				run_builtins(char **args, t_mshell *obj, int is_quote_heredoc);
int				cd(char **cd_args, t_mshell *obj);
int				pwd(t_mshell *obj);
int				env(t_mshell *obj, char **args);
int				echo(char **args, t_mshell *obj, int is_quote);
int				export(char **args, t_mshell *obj);
int				unset(char **args, t_mshell *obj);
void			check_and_handle_exit(char **args, t_mshell *obj);
void			getcwd_and_check(t_mshell *obj, char *buf);
void			update_pwd(t_mshell *obj, char *pwd, char *var);
void			handle_empty_cd(t_mshell *obj, char *buf);
int				handle_prev_path(t_mshell *obj);

/* ===== AST CORE (ast_core.c) ===== */

t_ast_node		*create_redir_args(t_ast_node *redir, char *filename);
t_ast_node		*handle_redirection_node(t_token **tokens);
t_ast_node		**append_redir(t_ast_node **redirs, t_ast_node *redir);
t_ast_node		*handle_redir_token(t_ast_node *cmd_node, t_token **tokens);
t_ast_node		*parse_simple_command(t_token **tokens);
t_ast_node		*parse_command(t_token **tokens);
t_ast_node		*parse_pipeline(t_token **tokens);
t_ast_node		*handle_word_token(t_ast_node *cmd_node, t_token **tokens);
t_ast_node		*create_pipe_structure(t_ast_node *root, t_token **tokens);
t_ast_node		*free_ast_return_null(t_ast_node **node);
t_ast_node		*create_ast_node(t_token_type type);

int				is_redirect_token(t_token_type type);
int				print_syntax_error(t_token *token, char *message);
int				print_newline_error(t_token *token);
int				validate_consecutive_redirects(t_token *token);
int				validate_redirection(t_token *token);
int				validate_pipe(t_token *token);
char			**append_arg(char **args, char *new_arg);
char			*get_redir_token_str(t_token_type type);
void			free_ast(t_ast_node *node);

/* ===== EXECUTION ===== */
void			print_exit(char *mes, char *cmd, t_mshell *obj);
char			*check_paths_access(char **paths, t_ast_node *node,
					t_mshell *obj);
void			execute_cmd(t_mshell *obj, t_ast_node *cmd);
void			choose_actions(t_mshell *obj);
void			exit_child(t_mshell *obj, char *arg, int exit_code,
					int is_builtin);
void			wait_for_children(t_mshell *obj);
void			alloc_pipes(t_mshell *obj);

/* ===== HEREDOC ===== */
void			run_heredoc(t_mshell *obj, t_ast_node *node);
void			write_heredoc_line(t_heredoc *doc);
t_heredoc		init_heredoc(t_mshell *obj);
void			cleanup_heredoc(t_heredoc *doc);
void			choose_heredoc_cmd(t_mshell *obj, t_ast_node *node);
void			alloc_run_heredoc(t_mshell *obj, t_ast_node *node);

/* ===== REDIRECTION ===== */
void			redirection_input(t_mshell *obj, t_ast_node *node);
void			redirection_output(t_mshell *obj, t_ast_node *node);
void			pipe_redirection(t_mshell *obj, t_ast_node *cmd);
int				handle_here_doc(t_mshell *obj, t_ast_node *node, int last_fd);
void			check_redirections(t_mshell *obj);
void			apply_redirections(t_mshell *obj, t_ast_node *cmd);

/* ===== CLEANUP ===== */
void			clean_exit(t_mshell *obj);
void			clean_mshell(t_mshell *obj);
void			close_fds(t_mshell *obj);
void			check_free_str(char **path);
void			close_heredoc_fds(t_mshell *obj);

/* ===== SIGNALS ===== */
void			setup_interactive_signals(void);
void			setup_exec_signals(void);
void			setup_heredoc_signals(void);
void			transition_signal_handlers(t_signal_state new_state);
void			disable_echoctl(void);
int				handle_heredoc_sigint(t_mshell *obj, int ret_fd,
					t_heredoc *heredoc);
void			interactive_sigint_handler(int signum);
void			heredoc_sigint_handler(int signum);
void			set_sigaction(int signum, void (*handler)(int), int flags);

#endif