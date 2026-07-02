#ifndef RPN_HPP
# define RPN_HPP

# include <iostream>
# include <stack>

class RPN
{
    private:
        RPN();
        ~RPN();
        RPN(const RPN& other);
        RPN& operator=(const RPN& other);
        static std::string removeWhitespaces(const std::string& str);
        static double calculate(double a, double b, char c);
    public:
        class InvalidInputException : public std::exception
        {
            public:
                std::string message;
                InvalidInputException(const std::string& msg);
                ~InvalidInputException() throw();
                const char* what() const throw();
        };

        static void calculate(const std::string& expression);
};

#endif