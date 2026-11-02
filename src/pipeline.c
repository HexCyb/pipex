#include "pipex.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* stdin of the first command: the infile, or the here_doc text */
static int	open_input(t_pipex *px)
{
	int	fd;

	if (px->heredoc_fd >= 0)
		return (px->heredoc_fd);
	fd = open(px->infile, O_RDONLY);
	if (fd < 0)
	{
		put_error(px->infile, strerror(errno));
		exit(1);
	}
	return (fd);
}

/* stdout of the last command: "> outfile", or ">> outfile" for here_doc */
static int	open_output(t_pipex *px)
{
	int	flags;
	int	fd;

	flags = O_WRONLY | O_CREAT | O_TRUNC;
	if (px->append)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	fd = open(px->outfile, flags, 0644);
	if (fd < 0)
	{
		put_error(px->outfile, strerror(errno));
		exit(1);
	}
	return (fd);
}

/* runs in the child: plug stdin/stdout, close the rest, become the command */
static void	child(t_pipex *px, int i, int in_fd, int *pipe_fd)
{
	int	out_fd;

	if (i < px->count - 1)
		close(pipe_fd[0]);
	if (i == 0)
		in_fd = open_input(px);
	if (i == px->count - 1)
		out_fd = open_output(px);
	else
		out_fd = pipe_fd[1];
	if (dup2(in_fd, STDIN_FILENO) < 0 || dup2(out_fd, STDOUT_FILENO) < 0)
		exit(1);
	close(in_fd);
	close(out_fd);
	exec_command(px->cmds[i], px->envp);
}

/* like the shell: the exit status is the one of the last command */
static int	wait_all(pid_t last)
{
	int		status;
	int		last_status;
	pid_t	pid;

	last_status = 1;
	pid = wait(&status);
	while (pid > 0)
	{
		if (pid == last && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (pid == last && WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		pid = wait(&status);
	}
	return (last_status);
}

int	run_pipeline(t_pipex *px)
{
	int		i;
	int		in_fd;
	int		pipe_fd[2];
	pid_t	pid;

	i = -1;
	in_fd = -1;
	while (++i < px->count)
	{
		if (i < px->count - 1 && pipe(pipe_fd) < 0)
			return (put_error("pipe", strerror(errno)), 1);
		pid = fork();
		if (pid < 0)
			return (put_error("fork", strerror(errno)), 1);
		if (pid == 0)
			child(px, i, in_fd, pipe_fd);
		if (in_fd >= 0)
			close(in_fd);
		if (i == 0 && px->heredoc_fd >= 0)
			close(px->heredoc_fd);
		in_fd = -1;
		if (i < px->count - 1)
		{
			close(pipe_fd[1]);
			in_fd = pipe_fd[0];
		}
	}
	return (wait_all(pid));
}
