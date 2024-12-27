# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/09 12:42:27 by msavelie          #+#    #+#              #
#    Updated: 2024/12/27 16:59:18 by msavelie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Colors

DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

NAME = minishell

SRC_DIR = ./srcs

SRCS = \
	${SRC_DIR}/main.c \
	${SRC_DIR}/parse.c \
	${SRC_DIR}/errors.c \
	${SRC_DIR}/clean.c \
	${SRC_DIR}/built_ins.c \
	${SRC_DIR}/ast.c \
	${SRC_DIR}/execution.c \
	${SRC_DIR}/validation.c

OBJS = ${SRCS:.c=.o}

CFLAGS = -g -Wall -Werror -Wextra
HEADERS = -L/usr/local/lib -I/usr/local/include -lreadline

LIBFT_NAME = ./libft_updated/libft.a
LIBFT_DIR = ./libft_updated

RM = rm -rf

.PHONY = all clean fclean re bonus

all: ${LIBFT_NAME} ${NAME}

${LIBFT_NAME}:
	@make -C ${LIBFT_DIR} --no-print-directory

${NAME}: ${OBJS}
	@echo "$(MAGENTA)🗂  Copying libft 🗂$(DEF_COLOR)"
	@cp ${LIBFT_DIR}/libft.a .
	@echo "$(BLUE)🛠  Compiling minishell... 🛠$(DEF_COLOR)"
	@cc ${CFLAGS} ${OBJS} libft.a $(HEADERS) -o ${NAME}
	@echo "$(GREEN)🥳 Success!🥳$(DEF_COLOR)"

%.o: %.c
	@cc $(CFLAGS) -c $< -o $@

clean:
	@echo "$(YELLOW)🚽 Deleting object files... 🚽$(DEF_COLOR)"
	@${RM} ${OBJS}
	@${RM} ${BONUS_OBJS}
	@make clean -C ${LIBFT_DIR} --no-print-directory

fclean: clean
	@make fclean -C ${LIBFT_DIR} --no-print-directory
	@${RM} libft.a
	@echo "$(RED)🪦 Deleting minishell... 🪦$(DEF_COLOR)"
	@${RM} ${NAME}
	@echo "$(RED)☣️  CLEAR ☣️$(DEF_COLOR)"
	@${RM} .bonus

re: fclean all
