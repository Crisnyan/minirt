/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vperez-f <vperez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/08 13:17:34 by vperez-f          #+#    #+#             */
/*   Updated: 2024/02/22 20:44:17 by vperez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10
# endif

int		check_nl_gnl(char *s, int *flag);
int		strlen_gnl(char *s);
int		read_buff_gnl(int fd, char **buffer);
int		check_after_line(char **after_l, char **buffer, int *r_flag, int *st);

char	*get_next_line(int fd);
char	*strjoin_gnl(char **buffer, char **tmp, int read_flag);
char	*substr_gnl(char **s, int start, int self, int *read_flag);
char	*get_line_gnl(char **buffer, int m_size, int self, int *read_flag);
char	*strdup_gnl(char **src);

void	free_stuff(char **s1, char **s2, char **s3);

#endif