#include "pipex.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

static char	*grow(char *buf, size_t len, size_t *cap)
{
	char	*bigger;
	size_t	i;

	*cap *= 2;
	bigger = malloc(*cap);
	if (bigger)
	{
		i = 0;
		while (i < len)
		{
			bigger[i] = buf[i];
			i++;
		}
	}
	free(buf);
	return (bigger);
}

/* one line from stdin, '\n' included; NULL at end of input */
static char	*read_line(void)
{
	char	*buf;
	size_t	len;
	size_t	cap;
	char	c;

	cap = 64;
	len = 0;
	buf = malloc(cap);
	while (buf && read(STDIN_FILENO, &c, 1) == 1)
	{
		if (len + 2 > cap)
			buf = grow(buf, len, &cap);
		if (!buf)
			return (NULL);
		buf[len++] = c;
		if (c == '\n')
			break ;
	}
	if (buf && len > 0)
	{
		buf[len] = '\0';
		return (buf);
	}
	free(buf);
	return (NULL);
}

static int	is_limiter(const char *line, const char *limiter)
{
	size_t	len;

	len = ft_strlen(limiter);
	if (ft_strncmp(line, limiter, len) != 0)
		return (0);
	return (line[len] == '\n' || line[len] == '\0');
}

/* reads lines until LIMITER into a temporary file and returns it opened */
int	read_heredoc(const char *limiter)
{
	int		fd;
	char	*line;

	fd = open(HEREDOC_TMP, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
		return (-1);
	while (1)
	{
		write(STDOUT_FILENO, "heredoc> ", 9);
		line = read_line();
		if (!line || is_limiter(line, limiter))
			break ;
		write(fd, line, ft_strlen(line));
		free(line);
	}
	free(line);
	close(fd);
	fd = open(HEREDOC_TMP, O_RDONLY);
	unlink(HEREDOC_TMP);
	return (fd);
}
