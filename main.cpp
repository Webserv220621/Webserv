# include "common.hpp"
#include "Webserv.hpp"

int		main(int ac, char **av)
{
    Webserv webserv;

	if (ac == 2)
	{
        webserv.read_config(av[1]);
        webserv.server_run();
	}
    else if (ac == 1) 
    {
        webserv.read_config("default.conf");
    }
	else
	{
		std::cout << "Invalid arg." << std::endl;
		return (1);
	}
	return (0);
}