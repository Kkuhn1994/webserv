#include "../include/webserv.hpp"

int main(int argc, char  *argv[])
{
	if (argc > 2) // by subject either have the config as argument or default path i think we need to do both
	{
		std::cerr << "Error: invalid number of arguments, please enter if used only the path to the config file\n";
		exit(EXIT_FAILURE);
	}
	else
	{
		FILE *file;
		if (argc == 2)
			file = fopen(argv[1], "r");
		else
			file = fopen(DEFAULT_CONFIG, "r");
		if (!file)
		{
			std::cerr << "Error opening file: " << argv[1] << std::endl;
			exit(EXIT_FAILURE);
		}
		fclose(file);
	}
		try
		{
			if(argc == 2) 
			{
				WebServer server(argv[1]);
				server.openSockets();
			}
			else 
			{
				WebServer server(DEFAULT_CONFIG);
				server.openSockets();
			}
			
		}
		catch(const std::exception& e)
		{
			std::cerr << "\e[5;2;38;240;23;23m" << "[WBSRV] ERROR: " << e.what() << "\e[0m" << std::endl;
		}


}
