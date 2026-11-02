#include "pipex.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void	put_error(const char *who, const char *what)
{
	write(STDERR_FILENO, "pipex: ", 7);
	write(STDERR_FILENO, who, ft_strlen(who));
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, what, ft_strlen(what));
	write(STDERR_FILENO, "\n", 1);
}

static char	**path_dirs(char **envp)
{
	int	i;

	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (split_char(envp[i] + 5, ':'));
		i++;
	}
	return (NULL);
}

static int	has_slash(const char *s)
{
	while (*s)
	{
		if (*s == '/')
			return (1);
		s++;
	}
	return (0);
}

/* "ls" -> "/usr/bin/ls" using PATH; names with a '/' are used as they are */
char	*find_command(const char *name, char **envp)
{
	char	**dirs;
	char	*full;
	int		i;

	if (name[0] == '\0')
		return (NULL);
	if (has_slash(name))
		return (ft_strdup(name));
	dirs = path_dirs(envp);
	i = 0;
	while (dirs && dirs[i])
	{
		full = ft_strjoin3(dirs[i], "/", name);
		if (full && access(full, X_OK) == 0)
		{
			free_tab(dirs);
			return (full);
		}
		free(full);
		i++;
	}
	free_tab(dirs);
	return (NULL);
}

/* never returns: execve() replaces the process, or the child exits 126/127 */
void	exec_command(char *cmd, char **envp)
{
	char	**args;
	char	*path;
	int		error;

	args = split_args(cmd);
	if (!args || !args[0])
	{
		put_error(cmd, "command not found");
		free_tab(args);
		exit(127);
	}
	path = find_command(args[0], envp);
	if (!path)
	{
		put_error(args[0], "command not found");
		free_tab(args);
		exit(127);
	}
	execve(path, args, envp);
	error = errno;
	put_error(args[0], strerror(error));
	free(path);
	free_tab(args);
	if (error == ENOENT)
		exit(127);
	exit(126);
}
