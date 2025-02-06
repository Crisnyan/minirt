/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vperez-f <vperez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:17:25 by vperez-f          #+#    #+#             */
/*   Updated: 2024/05/07 17:02:38 by vperez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	free_stuff(char **s1, char **s2, char **s3)
{
	if (s1 && *s1)
	{
		free(*s1);
		*s1 = NULL;
	}
	if (s2 && *s2)
	{
		free(*s2);
		*s2 = NULL;
	}
	if (s3 && *s3)
	{
		free(*s3);
		*s3 = NULL;
	}
	return ;
}

int	check_nl_gnl(char *s, int *read_flag)
{
	int	i;

	i = 0;
	if (s == NULL || *read_flag == -1)
	{
		*read_flag = -1;
		return (-1);
	}
	while (s[i] != '\0')
	{
		if (s[i] == '\n')
		{
			return (i + 1);
		}
		i++;
	}
	return (0);
}

int	read_buff_gnl(int fd, char **buffer)
{
	int		bytes_read;

	*buffer = (char *)malloc((sizeof(char) * BUFFER_SIZE) + 1);
	if (!*buffer)
		return (-1);
	bytes_read = read(fd, *buffer, BUFFER_SIZE);
	if (bytes_read < 0)
	{
		free_stuff(NULL, buffer, NULL);
		return (-1);
	}
	if (bytes_read == 0)
	{
		free_stuff(NULL, buffer, NULL);
		return (1);
	}
	(*buffer)[bytes_read] = '\0';
	return (0);
}

int	check_after_line(char **after_l, char **buffer, int *r_f, int *start)
{
	int	n;

	*start = 0;
	*r_f = 0;
	*buffer = NULL;
	n = 0;
	if (after_l && *after_l)
	{
		if (check_nl_gnl(*after_l, &n))
		{
			*buffer = get_line_gnl(after_l, check_nl_gnl(*after_l, &n), 0, r_f);
			*after_l = substr_gnl(after_l, check_nl_gnl(*after_l, &n), 1, r_f);
			return (1);
		}
		else
		{
			*buffer = strdup_gnl(after_l);
			return (0);
		}
	}
	return (0);
}

char	*get_next_line(int fd)
{
	static char	*after_line = NULL;
	char		*buffer;
	char		*tmp;
	int			read_flag;
	int			start;

	if (fd < 0 || BUFFER_SIZE < 1)
		return (NULL);
	if (check_after_line(&after_line, &buffer, &read_flag, &start))
		return (buffer);
	while (!read_flag && !start)
	{
		tmp = strdup_gnl(&buffer);
		read_flag = read_buff_gnl(fd, &buffer);
		buffer = strjoin_gnl(&buffer, &tmp, read_flag);
		start = check_nl_gnl(buffer, &read_flag);
	}
	after_line = substr_gnl(&buffer, start, 0, &read_flag);
	buffer = get_line_gnl(&buffer, start, 1, &read_flag);
	if (read_flag == -1)
	{
		free_stuff(&after_line, &buffer, &tmp);
		return (NULL);
	}
	return (buffer);
}
/*
int	main(void)
{
	int	fd;
	int i;
	char *res;

	i = 1;
	fd = open("test.txt", O_RDONLY);
	res = get_next_line(fd);
	while (res != NULL)
	{
		printf("Line[%i]: %s\n", i, res);
		free(res);
		res = get_next_line(fd);
	}
	printf("Line[%i]: %s\n", i, res);
	free(res);

	close(fd);
}*/
