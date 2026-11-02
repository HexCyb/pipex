#include "pipex.h"
#include <stdlib.h>

size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

int	ft_strncmp(const char *a, const char *b, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (a[i] || b[i]))
	{
		if (a[i] != b[i])
			return ((unsigned char)a[i] - (unsigned char)b[i]);
		i++;
	}
	return (0);
}

char	*ft_strdup(const char *s)
{
	return (ft_strjoin3(s, "", ""));
}

char	*ft_strjoin3(const char *a, const char *b, const char *c)
{
	char	*out;
	size_t	i;
	size_t	j;

	out = malloc(ft_strlen(a) + ft_strlen(b) + ft_strlen(c) + 1);
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	while (a[j])
		out[i++] = a[j++];
	j = 0;
	while (b[j])
		out[i++] = b[j++];
	j = 0;
	while (c[j])
		out[i++] = c[j++];
	out[i] = '\0';
	return (out);
}
