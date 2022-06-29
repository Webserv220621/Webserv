# include "common.hpp"
# include "Location.hpp"

class Server 
{
    public :
        void run(void);
    private :
        std::map<int, Location> locations;
        
}