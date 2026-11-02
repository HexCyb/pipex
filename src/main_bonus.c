#include "pipex.h"
#include <errno.h>
#include <string.h>

static int	usage(void)
{
	put_error("usage", "./pipex infile \"cmd1\" ... \"cmdn\" outfile");
	put_error("usage", "./pipex here_doc LIMITER \"cmd1\" ... \"cmdn\" outfile");
	return (1);
}

/*
** ./pipex infile c1 c2 ... cn outfile     ==  < infile c1 | c2 | ... | cn > outfile
** ./pipex here_doc END c1 ... cn outfile   ==  c1 << END | ... | cn >> outfile
*/
int	main(int argc, char **argv, char **envp)
{
	t_pipex	px;
	int		first;

	px.heredoc_fd = -1;
	px.append = 0;
	first = 2;
	if (argc >= 6 && ft_strncmp(argv[1], "here_doc", 9) == 0)
	{
		px.heredoc_fd = read_heredoc(argv[2]);
		if (px.heredoc_fd < 0)
		{
			put_error("here_doc", strerror(errno));
			return (1);
		}
		px.append = 1;
		first = 3;
	}
	else if (argc < 5)
		return (usage());
	px.infile = argv[1];
	px.cmds = argv + first;
	px.count = argc - first - 1;
	px.outfile = argv[argc - 1];
	px.envp = envp;
	return (run_pipeline(&px));
}
