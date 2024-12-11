#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft_updated/libft.h"
# include <stdio.h>

typedef struct s_command t_command; // forward declaration

typedef struct	s_mshell
{
	char	*cmd_line; //(rename to *input_line?)
	char	**paths;
	char	*cur_path;//(rename to current_path?)
	int		is_heredoc;//(rename to heredoc_active?)
	int		total_cmds;//(rename to command_count?)
	int		allocated_pipes;//(rename to pipe_count?)
	int		**pipfd;//(rename to **pipe_fds?)
	struct	s_command *commands;
}	t_mshell;

int		error_ret(int type, char *arg);
char	**fetch_paths(char **envp);
void	parse(t_mshell *obj);
void	clean_mshell(t_mshell *obj);

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
    t_token_type      type;
    char              *value;
    struct s_token    *next;
}   t_token;


typedef struct s_ast_node
{
    t_token_type        type;       // Type: TOKEN_PIPE, TOKEN_REDIRECT, TOKEN_WORD
    char                **args;     // Command arguments (only for commands)
    struct s_ast_node   *left;      // Left child (e.g., left command in a pipe)
    struct s_ast_node   *right;     // Right child (e.g., right command in a pipe)
    //t_redirection       *redir;     // Redirection details (if any)
}   t_ast_node;

#endif
