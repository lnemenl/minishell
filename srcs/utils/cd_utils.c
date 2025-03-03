/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 15:04:22 by msavelie          #+#    #+#             */
/*   Updated: 2025/03/03 12:47:42 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	handle_empty_cd(t_mshell *obj, char *buf)
{
	char	*home_path;

	obj->exit_code = 0;
	home_path = get_env_var(obj->envp, "HOME=");
	if (!home_path)
	{
		obj->exit_code = 1;
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		return ;
	}
	if (obj->prev_path)
		free(obj->prev_path);
	obj->prev_path = ft_strdup(buf);
	update_pwd(obj, buf, "OLDPWD=");
	chdir(home_path);
	update_pwd(obj, home_path, "PWD=");
	if (home_path)
		free(home_path);
}

int	handle_prev_path(t_mshell *obj)
{
	if (obj->prev_path)
		free(obj->prev_path);
	obj->prev_path = get_env_var(obj->envp, "OLDPWD=");
	if (!obj->prev_path)
	{
		obj->exit_code = 1;
		ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
		return (0);
	}
	if (chdir(obj->prev_path) == -1)
	{
		obj->exit_code = 1;
		concat_print("minishell: cd: ", obj->prev_path,
			": No such file or directory", obj);
		return (0);
	}
	printf("%s\n", obj->prev_path);
	return (1);
}
