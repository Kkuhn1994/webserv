NAME		=	webserv

CC				=	c++
CXXFLAGS		=	-Werror -Wextra -Wall -I $(HEADER_DIR)

HEADER_DIR		=	./include/
OBJ_DIR		=	./obj/
SRC_DIR		=	./src/
SOURCES		=	$(addprefix $(SRC_DIR),$(addsuffix .cpp,$(FILES)))
OBJECTS		=	$(addprefix $(OBJ_DIR),$(addsuffix .o,$(FILES)))

FILES	=	\
			ConfService \
			ServerBlock \
			Server \
			main


$(NAME): $(OBJECTS)
	@$(CC) $(CXXFLAGS) $(SOURCES) -o $@

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CXXFLAGS) -c $< -o $@

all: $(NAME)

clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf conf/*ServerConfig*
	@rm -rf conf/*locationConfig*
	@rm -rf conf/*tempConfig*

fclean: clean
	@rm -f $(NAME)
	@rm -rf conf/*ServerConfig*
	@rm -rf conf/*locationConfig*
	@rm -rf conf/*tempConfig*

re: fclean all

.PHONY: all clean fclean make