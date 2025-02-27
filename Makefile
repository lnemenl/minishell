# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: msavelie <msavelie@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/09/09 12:42:27 by msavelie          #+#    #+#              #
#    Updated: 2025/02/27 15:37:21 by msavelie         ###   ########.fr        #
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
BROWN = \033[0;33m

NAME = minishell

AST = ./abstract_syntax_tree
BUILTINS = ./builtins
EXEC = ./execution
PARSE = ./parsing
SIG = ./signals
SRC_DIR = ./srcs
UTILS = ./utils

SRCS = \
	${SRC_DIR}/clean_fds.c \
	${SRC_DIR}/clean.c \
	${SRC_DIR}/errors.c \
	${SRC_DIR}/main.c \
	${SRC_DIR}/validation.c \
	\
	${SRC_DIR}/${UTILS}/cd_utils.c \
	${SRC_DIR}/${UTILS}/envp_utils.c \
	${SRC_DIR}/${UTILS}/envp_utils_2.c \
	${SRC_DIR}/${UTILS}/envp_utils_3.c \
	${SRC_DIR}/${UTILS}/execution_utils.c \
	${SRC_DIR}/${UTILS}/heredoc_utils.c \
	${SRC_DIR}/${UTILS}/pipes_allocation.c \
	${SRC_DIR}/${UTILS}/redirection_utils.c \
	\
	${SRC_DIR}/${PARSE}/parse_construction.c \
	${SRC_DIR}/${PARSE}/parse_env_expansion.c \
	${SRC_DIR}/${PARSE}/parse_env_string_utils.c \
	${SRC_DIR}/${PARSE}/parse_invalid_cases.c \
	${SRC_DIR}/${PARSE}/parse_operator_and_word.c \
	${SRC_DIR}/${PARSE}/parse_token_core.c \
	${SRC_DIR}/${PARSE}/parse_token_env.c \
	${SRC_DIR}/${PARSE}/parse_token_process.c \
	${SRC_DIR}/${PARSE}/parse_token_quote.c \
	${SRC_DIR}/${PARSE}/parse_token_utils.c \
	${SRC_DIR}/${PARSE}/parse.c \
	${SRC_DIR}/${PARSE}/parsing_utils.c \
	\
	${SRC_DIR}/${AST}/ast_command.c \
	${SRC_DIR}/${AST}/ast_core.c \
	${SRC_DIR}/${AST}/ast_cleaning.c \
	${SRC_DIR}/${AST}/ast_redirs.c \
	${SRC_DIR}/${AST}/ast_syntax_errors.c \
	${SRC_DIR}/${AST}/ast_validating.c \
	\
	${SRC_DIR}/${BUILTINS}/built_ins.c \
	${SRC_DIR}/${BUILTINS}/cd.c \
	${SRC_DIR}/${BUILTINS}/echo.c \
	${SRC_DIR}/${BUILTINS}/env.c \
	${SRC_DIR}/${BUILTINS}/exit.c \
	${SRC_DIR}/${BUILTINS}/export.c \
	${SRC_DIR}/${BUILTINS}/pwd.c \
	${SRC_DIR}/${BUILTINS}/unset.c \
	\
	${SRC_DIR}/${EXEC}/execution.c \
	${SRC_DIR}/${EXEC}/heredoc.c \
	${SRC_DIR}/${EXEC}/redirection.c \
	\
	${SRC_DIR}/${SIG}/signals_handling.c \
	${SRC_DIR}/${SIG}/signals_heredoc.c \
	${SRC_DIR}/${SIG}/signals_settingup.c

OBJS = ${SRCS:.c=.o}

CFLAGS = -g -Wall -Werror -Wextra
INCLUDES = -I/usr/local/include
LDFLAGS = -L/opt/homebrew/opt/readline/lib -lreadline

LIBFT_NAME = ./libft_updated/libft.a
LIBFT_DIR = ./libft_updated

RM = rm -rf

.PHONY = all clean fclean re

all: ${LIBFT_NAME} ${NAME}

${LIBFT_NAME}:
	@make -C ${LIBFT_DIR} --no-print-directory

${NAME}: ${OBJS}
	@echo "$(MAGENTA)🗂  Copying libft 🗂$(DEF_COLOR)"
	@cp ${LIBFT_DIR}/libft.a .
	@echo "$(BROWN)💩 Compiling shit-shell... 💩$(DEF_COLOR)"
	@cc ${CFLAGS} ${OBJS} libft.a $(LDFLAGS) -o ${NAME}
	@echo "$(GREEN)🥳 Success!🥳$(DEF_COLOR)"

%.o: %.c
	@cc $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "$(YELLOW)🚽 Deleting object files... 🚽$(DEF_COLOR)"
	@${RM} ${OBJS}
	@make clean -C ${LIBFT_DIR} --no-print-directory

fclean: clean
	@make fclean -C ${LIBFT_DIR} --no-print-directory
	@${RM} libft.a
	@echo "$(RED)🪦 Deleting minishell... 🪦$(DEF_COLOR)"
	@${RM} ${NAME}
	@echo "$(RED)☣️  CLEAR ☣️$(DEF_COLOR)"

re: fclean all
