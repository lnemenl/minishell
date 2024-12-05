#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft_updated/libft.h"
# include <stdio.h>

typedef struct	mshell
{
	char	*cmd_line;
	char	**cmds;
	char	**paths;
	char	*cur_path;
	int		is_heredoc;
	int		total_cmds;
	int		allocated_pipes;
	int		**pipfd;
} t_mshell;

int		error_ret(int type, char *arg);
char	**fetch_paths(char **envp);
void	parse(t_mshell *obj);
void	clean_mshell(t_mshell *obj);

#endif