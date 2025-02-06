/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vperez-f <vperez-f@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/09 13:09:18 by vperez-f          #+#    #+#             */
/*   Updated: 2024/09/02 15:57:39 by vperez-f         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strappend(char **s1, char const *s2)
{
	int		maxlen;
	int		i;
	int		j;
	char	*str;

	i = 0;
	j = 0;
	if (!s1)
		return (NULL);
	if (!s2)
		return (*s1);
	if (!*s1)
		return (ft_strdup(s2));
	maxlen = ft_strlen(*s1) + ft_strlen(s2);
	str = (char *)malloc(maxlen + 1);
	if (str == 0)
		return (NULL);
	while ((*s1)[i] != '\0')
		str[j++] = (*s1)[i++];
	i = 0;
	while (s2[i] != '\0')
		str[j++] = s2[i++];
	str[j] = '\0';
	free(*s1);
	return (str);
}

char	*ft_strattach(char const *s1, char **s2)
{
	int		maxlen;
	int		i;
	int		j;
	char	*str;

	i = 0;
	j = 0;
	if (!s1 || !s2 || !*s2)
		return (NULL);
	maxlen = ft_strlen(s1) + ft_strlen(*s2);
	str = (char *)malloc(maxlen + 1);
	if (str == 0)
		return (NULL);
	while (s1[i] != '\0')
	{
		str[j++] = s1[i++];
	}
	i = 0;
	while ((*s2)[i] != '\0')
	{
		str[j++] = (*s2)[i++];
	}
	str[j] = '\0';
	free(*s2);
	return (str);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	int		maxlen;
	int		i;
	int		j;
	char	*str;

	i = 0;
	j = 0;
	maxlen = ft_strlen(s1) + ft_strlen(s2);
	str = (char *)malloc(maxlen + 1);
	if (str == 0)
		return (NULL);
	while (s1[i] != '\0')
	{
		str[j++] = s1[i++];
	}
	i = 0;
	while (s2[i] != '\0')
	{
		str[j++] = s2[i++];
	}
	str[j] = '\0';
	return (str);
}

/*
int main (void)
{
	char	a[] = "asdad";
	char	b[] = "Hello";
	printf("s: %s\n", ft_strjoin(a,b));
}
*/
