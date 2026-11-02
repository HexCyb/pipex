# pipex

Shell pipes rebuilt in C with `pipe`, `fork`, `dup2` and `execve`. The program behaves like the shell, error messages and exit codes included.

> Originally completed at **42 Türkiye** as part of the common core. This repository is a 2026 rewrite, because the original code was lost.
> 42 Türkiye'de tamamladığım projenin 2026'da yeniden yazılmış hali; orijinal kod kayboldu.

## Usage

```sh
make                 # mandatory part
./pipex infile "grep hello" "wc -l" outfile
# same as:   < infile grep hello | wc -l > outfile

make bonus           # any number of commands + here_doc
./pipex infile "cat" "grep hello" "tr a-z A-Z" "wc -l" outfile
# same as:   < infile cat | grep hello | tr a-z A-Z | wc -l > outfile

./pipex here_doc END "cat" "wc -l" outfile
# same as:   cat << END | wc -l >> outfile
```

Commands can be quoted like in the shell: `"awk '{print \$1}'"` becomes `awk` + `{print $1}`.

## How it works

```
 infile ──▶ [ cmd1 ] ──pipe──▶ [ cmd2 ] ──pipe──▶ ... ──▶ [ cmdN ] ──▶ outfile
           child 1            child 2                    child N
```

1. For every command the parent creates a pipe (except for the last command) and `fork`s a child.
2. The child connects its stdin to the infile or the previous pipe, and its stdout to the next pipe or the outfile, using `dup2`. It closes every other descriptor and calls `execve`.
3. The command is found by searching each folder in `PATH`. A name that contains `/` is used as given.
4. The parent closes its copies of the pipe ends right away. Otherwise a command could wait forever for an EOF that never comes.
5. The parent waits for every child and returns the **last** command's exit status, like the shell does.

## Behaves like bash

| Situation | Result |
|---|---|
| infile missing | error message, that command does not run, the rest of the pipeline still runs |
| command not found | `pipex: cmd: command not found`, exit 127 |
| not executable | error message, exit 126 |
| outfile not writable | error message, exit 1 |
| no `PATH` in the environment | command not found, exit 127 |
| `here_doc` | the text is read up to the limiter line, and the outfile is appended to (`>>`) |

`make test` compares pipex with bash on 14 cases (same output file, same exit status). Valgrind reports no leaks.

## Files

```
include/pipex.h
src/main.c            mandatory: exactly 2 commands
src/main_bonus.c      bonus: n commands, here_doc
src/pipeline.c        pipes, forks, redirections, waiting
src/exec.c            PATH search, execve, error messages
src/split.c           splitting a command into arguments (quotes too) and PATH into folders
src/heredoc_bonus.c   reading the here_doc text
src/utils.c           small string helpers
tests/run_tests.sh    side-by-side comparison with bash
```
