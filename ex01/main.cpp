#include "RPN.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <expression>" << std::endl;
        return 1;
    }

    try
    {
        RPN::calculate(argv[1]);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}