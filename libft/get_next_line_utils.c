/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vperez-f <vperez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:17:31 by vperez-f          #+#    #+#             */
/*   Updated: 2024/02/22 21:26:58 by vperez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*strdup_gnl(char **src)
{
	char	*dup;
	int		i;

	i = 0;
	if (!*src)
		return (NULL);
	dup = (char *)malloc((sizeof(char) * strlen_gnl(*src)) + 1);
	if (!dup)
	{
		free_stuff(src, NULL, NULL);
		return (NULL);
	}
	while ((*src)[i] != '\0')
	{
		dup[i] = (*src)[i];
		i++;
	}
	dup[i] = '\0';
	free_stuff(src, NULL, NULL);
	return (dup);
}

int	strlen_gnl(char *s)
{
	int	i;

	i = 0;
	if (!s)
		return (0);
	while (s[i] != '\0')
		i++;
	return (i);
}

char	*strjoin_gnl(char **buffer, char **tmp, int read_flag)
{
	int		i;
	int		j;
	char	*res;

	i = 0;
	j = 0;
	if (!*tmp || read_flag == -1)
		return (*buffer);
	res = (char *)malloc((strlen_gnl(*buffer) + strlen_gnl(*tmp)) + 1);
	if (!res)
		return (free_stuff(buffer, tmp, NULL), NULL);
	while ((*tmp)[i] != '\0')
	{
		res[i] = (*tmp)[i];
		i++;
	}
	while (*buffer && (*buffer)[j] != '\0')
	{
		res[i + j] = (*buffer)[j];
		j++;
	}
	res[i + j] = '\0';
	free_stuff(buffer, tmp, NULL);
	return (res);
}

char	*substr_gnl(char **s, int start, int self, int *read_flag)
{
	char			*substr;
	int				s_len;
	int				i;

	i = 0;
	if (!*s || !start || *read_flag == -1)
		return (NULL);
	s_len = strlen_gnl(*s);
	if (s_len < start)
		return (free_stuff(s, NULL, NULL), NULL);
	substr = (char *)malloc(sizeof(char) * ((s_len - start) + 1));
	if (!substr)
		return (free_stuff(s, NULL, NULL), NULL);
	while ((*s)[start + i] != '\0')
	{
		substr[i] = (*s)[start + i];
		i++;
	}
	substr[i] = '\0';
	if (i == 0)
		free_stuff(&substr, NULL, NULL);
	if (self)
		free_stuff(s, NULL, NULL);
	return (substr);
}

char	*get_line_gnl(char **buffer, int m_size, int self, int *read_flag)
{
	int		i;
	char	*line;

	i = 0;
	if (!*buffer || *read_flag == -1)
		return (*read_flag = -1, NULL);
	if (m_size == 0)
		m_size = strlen_gnl(*buffer);
	line = (char *)malloc((sizeof(char) * m_size) + 1);
	if (!line || m_size == 0)
	{
		free_stuff(buffer, &line, NULL);
		return (*read_flag = -1, NULL);
	}
	while ((*buffer)[i] != '\n' && (*buffer)[i] != '\0')
	{
		line[i] = (*buffer)[i];
		i++;
	}
	if ((*buffer)[i] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	if (self)
		free_stuff(buffer, NULL, NULL);
	return (line);
}
