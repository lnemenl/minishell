/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/18 15:36:40 by msavelie          #+#    #+#             */
/*   Updated: 2025/03/03 12:56:16 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static char	*handle_cd_path(char *provided_path, char *buf, t_mshell *obj)
{
	char	*ret_path;
	char	*path;

	if (*provided_path == '/')
		return (provided_path);
	path = ft_strjoin(buf, "/");
	if (!path)
		print_exit("Malloc error\n", "cd", obj);
	ret_path = ft_strjoin(path, provided_path);
	free(path);
	if (!ret_path)
		print_exit("Malloc error\n", "cd", obj);
	return (ret_path);
}

/* `var` must be provided with `=` */
void	update_pwd(t_mshell *obj, char *pwd, char *var)
{
	char	*joined_pwd;
	size_t	var_len;
	size_t	pwd_len;
	size_t	total_len;

	if (!pwd)
		return ;
	pwd_len = ft_strlen(pwd);
	var_len = ft_strlen(var);
	total_len = pwd_len + var_len + 1;
	joined_pwd = ft_calloc(total_len, sizeof(char));
	if (!joined_pwd)
		print_exit("Malloc error\n", NULL, obj);
	joined_pwd = ft_memmove(joined_pwd, var, var_len);
	ft_strlcat(joined_pwd, pwd, total_len);
	joined_pwd[total_len - 1] = '\0';
	put_env_var(obj, joined_pwd, "envp");
	put_env_var(obj, joined_pwd, "export");
	free(joined_pwd);
}

static int	check_cd_args(char **cd_args, t_mshell *obj, char *buf)
{
	if (!cd_args[1] || !*cd_args[1])
	{
		handle_empty_cd(obj, buf);
		return (1);
	}
	else if (cd_args[2] && *cd_args[2])
	{
		obj->exit_code = 1;
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	return (0);
}

static int	change_path(t_mshell *obj, char **cd_args,
	char *buf, char *full_path)
{
	char	*err_mes;

	if (ft_strcmp(buf, cd_args[1]) == 0)
		obj->exit_code = 0;
	else if (ft_strcmp(cd_args[1], "-") == 0)
	{
		if (handle_prev_path(obj) == 0)
			return (0);
	}
	else if (chdir(full_path) == -1)
	{
		obj->exit_code = 1;
		err_mes = ft_strjoin("minishell: cd: ", cd_args[1]);
		if (!err_mes)
			print_exit("Malloc error\n", NULL, obj);
		perror(err_mes);
		check_free_str(&err_mes);
		return (0);
	}
	else
		obj->exit_code = 0;
	return (1);
}

int	cd(char **cd_args, t_mshell *obj)
{
	char	buf[PATH_BUFFER_SIZE];
	char	*full_path;

	getcwd_and_check(obj, buf);
	if (check_cd_args(cd_args, obj, buf) == 1)
		return (1);
	full_path = handle_cd_path(cd_args[1], buf, obj);
	if (!full_path)
		return (1);
	if (change_path(obj, cd_args, buf, full_path) == 1)
	{
		if (obj->prev_path)
			free(obj->prev_path);
		obj->prev_path = ft_strdup(buf);
		update_pwd(obj, buf, "OLDPWD=");
	}
	getcwd_and_check(obj, buf);
	update_pwd(obj, buf, "PWD=");
	if (ft_strcmp(full_path, cd_args[1]) != 0)
		free(full_path);
	return (1);
}
