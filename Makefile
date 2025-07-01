SRC		= window.c
OBJ		= $(SRC:%.c=%.o)
CC		= cc
CFLAGS	= -Wall -Werror -Wextra -Imlx -Ilibft -g -fsanitize=address -fsanitize=undefined
LDFLAGS	= -Lmlx -Llibft -lmlx -lft -lXext -lX11 -lm -lz -g -fsanitize=address -fsanitize=undefined
NAME	= minirt

all: $(NAME)

$(NAME): $(OBJ) Makefile
	$(CC) $(OBJ) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -O3 -c $< -o $@
	@make -C mlx
	@make -C libft

clean:
	rm -rf $(OBJ)
	@make clean -C mlx
	@make clean -C libft

fclean:	clean
	rm -rf $(NAME)

good: all
	rm -rf $(OBJ)

re: fclean all

.PHONY: all clean fclean re good
