#include "pipex.h"
#include <stdlib.h>

void	free_tab(char **tab)
{
	int	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
}

/* one argument; quotes group words and are removed: 'a b' -> [a b] */
static char	*next_word(const char *s, size_t *i)
{
	char	*word;
	size_t	len;
	char	quote;

	word = malloc(ft_strlen(s) + 1);
	if (!word)
		return (NULL);
	len = 0;
	quote = 0;
	while (s[*i] && (quote || (s[*i] != ' ' && s[*i] != '\t')))
	{
		if (!quote && (s[*i] == '\'' || s[*i] == '"'))
			quote = s[*i];
		else if (quote && s[*i] == quote)
			quote = 0;
		else
			word[len++] = s[*i];
		(*i)++;
	}
	word[len] = '\0';
	return (word);
}

/* "awk '{print $1}'" -> ["awk", "{print $1}", NULL] */
char	**split_args(const char *s)
{
	char	**tab;
	size_t	i;
	size_t	n;

	tab = malloc(sizeof(char *) * (ft_strlen(s) / 2 + 2));
	if (!tab)
		return (NULL);
	i = 0;
	n = 0;
	while (s[i])
	{
		while (s[i] == ' ' || s[i] == '\t')
			i++;
		if (!s[i])
			break ;
		tab[n] = next_word(s, &i);
		if (!tab[n])
			break ;
		n++;
	}
	tab[n] = NULL;
	return (tab);
}

static char	*copy_part(const char *s, size_t len)
{
	char	*part;
	size_t	i;

	part = malloc(len + 1);
	if (!part)
		return (NULL);
	i = 0;
	while (i < len)
	{
		part[i] = s[i];
		i++;
	}
	part[len] = '\0';
	return (part);
}

/* "/usr/bin:/bin" -> ["/usr/bin", "/bin", NULL] (used for PATH) */
char	**split_char(const char *s, char c)
{
	char	**tab;
	size_t	n;
	size_t	len;

	tab = malloc(sizeof(char *) * (ft_strlen(s) + 2));
	if (!tab)
		return (NULL);
	n = 0;
	while (*s)
	{
		len = 0;
		while (s[len] && s[len] != c)
			len++;
		if (len > 0)
			tab[n++] = copy_part(s, len);
		s += len;
		if (*s == c)
			s++;
	}
	tab[n] = NULL;
	return (tab);
}
