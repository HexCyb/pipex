#ifndef PIPEX_H
# define PIPEX_H

# include <stddef.h>

# define HEREDOC_TMP "/tmp/.pipex_heredoc"

/*
** ./pipex infile cmd1 cmd2 ... outfile
** ./pipex here_doc LIMITER cmd1 cmd2 ... outfile   (bonus)
*/
typedef struct s_pipex
{
	char	**cmds;
	int		count;
	char	*infile;
	int		heredoc_fd;
	char	*outfile;
	int		append;
	char	**envp;
}	t_pipex;

int		run_pipeline(t_pipex *px);
void	exec_command(char *cmd, char **envp);
char	*find_command(const char *name, char **envp);
int		read_heredoc(const char *limiter);

char	**split_args(const char *s);
char	**split_char(const char *s, char c);
void	free_tab(char **tab);

size_t	ft_strlen(const char *s);
int		ft_strncmp(const char *a, const char *b, size_t n);
char	*ft_strdup(const char *s);
char	*ft_strjoin3(const char *a, const char *b, const char *c);
void	put_error(const char *who, const char *what);

#endif
