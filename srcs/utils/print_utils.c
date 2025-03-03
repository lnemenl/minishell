/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 12:19:47 by msavelie          #+#    #+#             */
/*   Updated: 2025/03/03 12:34:35 by msavelie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void	concat_print(char *arg1, char *arg2, char *arg3, t_mshell *obj)
{
	char	*full_msg;
	size_t	len;

	if (!arg1 || !arg2)
		return ;
	len = ft_strlen(arg1) + ft_strlen(arg2) + 2;
	if (arg3)
		len += ft_strlen(arg3);
	full_msg = ft_calloc(len, sizeof(char));
	if (!full_msg)
		print_exit("Malloc error\n", NULL, obj);
	ft_strlcat(full_msg, arg1, len);
	ft_strlcat(full_msg, arg2, len);
	if (arg3)
		ft_strlcat(full_msg, arg3, len);
	ft_strlcat(full_msg, "\n", len);
	ft_putstr_fd(full_msg, STDERR_FILENO);
	check_free_str(&full_msg);
}
