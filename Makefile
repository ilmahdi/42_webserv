# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eabdelha <eabdelha@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/15 08:46:38 by eabdelha          #+#    #+#              #
#    Updated: 2023/01/12 14:27:05 by eabdelha         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.PHONY : all clean fclean re 

CC = c++
CFLAGS = -Wall -Wextra -Werror
#-std=c++98
#-Wuninitialized -Wshadow -Wunused -Wformat -Wparentheses \
#-Wpedantic   -Wformat=2 -Wformat-security -Wnull-dereference -Wstack-protector -Wvla   \
#-g -fsanitize=address -fsanitize=undefined -fsanitize=integer -fno-omit-frame-pointer

NAME = webserv

SRC_PATH = ./src/
DEF_PATH = ./src/defs/
UTL_PATH = ./src/utils/

CLS_PATH = ./src/class/
INC_PATH = ./src/incl/

OBJ_PATH = ./obj/

INCLUDES =	$(INC_PATH)mini_structs.hpp			\
			$(INC_PATH)exceptions.hpp			\
			$(INC_PATH)macros.hpp				\
			$(INC_PATH)utils.hpp				\
			\
			$(CLS_PATH)ConfigParser.hpp			\
			$(CLS_PATH)ServerLauncher.hpp		\
			$(CLS_PATH)RecvHandler.hpp			\
			$(CLS_PATH)SendHandler.hpp			\
			$(CLS_PATH)RequestParser.hpp		\
			$(CLS_PATH)RequestProcessor.hpp		\
			$(CLS_PATH)CGIExecutor.hpp			\
			$(CLS_PATH)BodyUnchunker.hpp			\
			

SRC	=	main.cpp	\
		\
		ConfigParser.cpp		\
		ServerLauncher.cpp		\
		RecvHandler.cpp			\
		SendHandler.cpp			\
		RequestParser.cpp		\
		RequestProcessor.cpp	\
		CGIExecutor.cpp			\
		BodyUnchunker.cpp			\
		\
		utility.cpp				\
		build_response.cpp		\
		exceptions.cpp			\
	
OBJECTS = $(addprefix $(OBJ_PATH), $(SRC:.cpp=.o))

all: $(OBJ_PATH) $(NAME) $(STD_NAME)

$(NAME): $(OBJECTS) 
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)


$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_PATH)%.o: $(DEF_PATH)%.cpp $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_PATH)%.o: $(UTL_PATH)%.cpp $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<


$(OBJ_PATH) : 
	@mkdir $@

clean:
	@rm -rf  $(OBJ_PATH)

fclean: clean 
	@rm -f $(NAME) $(STD_NAME) 

re: fclean all
