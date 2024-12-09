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

#endif