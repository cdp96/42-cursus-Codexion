NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
RM = rm -f
INCLUDES = -Iinclude

# sources
SRC = src/main.c src/parsing.c src/utils.c src/init.c src/run.c \
	  src/time.c src/cleanup.c src/monitor.c src/coder_state.c \
	  src/dongles.c src/heap/heap.c src/heap/heap_utils.c \
	  src/dongle_acquire.c

# Object files
OBJS = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c include/
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
