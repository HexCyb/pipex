#include "pipex.h"

/* ./pipex infile "cmd1" "cmd2" outfile   ==   < infile cmd1 | cmd2 > outfile */
int	main(int argc, char **argv, char **envp)
{
	t_pipex	px;

	if (argc != 5)
	{
		put_error("usage", "./pipex infile \"cmd1\" \"cmd2\" outfile");
		return (1);
	}
	px.infile = argv[1];
	px.heredoc_fd = -1;
	px.cmds = argv + 2;
	px.count = 2;
	px.outfile = argv[4];
	px.append = 0;
	px.envp = envp;
	return (run_pipeline(&px));
}
