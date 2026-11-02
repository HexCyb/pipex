NAME     := pipex
CC       := cc
CFLAGS   := -Wall -Wextra -Werror -Iinclude

COMMON   := src/pipeline.c src/exec.c src/split.c src/utils.c
MANDATORY:= src/main.c
BONUS    := src/main_bonus.c src/heredoc_bonus.c

COMMON_O := $(COMMON:src/%.c=obj/%.o)
MAND_O   := $(MANDATORY:src/%.c=obj/%.o)
BONUS_O  := $(BONUS:src/%.c=obj/%.o)

all: $(NAME)

$(NAME): $(COMMON_O) $(MAND_O)
	$(CC) $(CFLAGS) $^ -o $@

bonus: $(COMMON_O) $(BONUS_O)
	$(CC) $(CFLAGS) $^ -o $(NAME)

obj/%.o: src/%.c include/pipex.h
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

test: bonus
	bash tests/run_tests.sh

.PHONY: all bonus clean fclean re test
