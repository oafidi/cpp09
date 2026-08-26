#include "RPN.hpp"

RPN::RPN() {}

RPN::~RPN() {}

RPN::RPN(const RPN& other) { (void)other; }

RPN& RPN::operator=(const RPN& other)
{
    (void)other;
    return *this;
}

RPN::InvalidInputException::InvalidInputException(const std::string& msg) : message(msg) {}

RPN::InvalidInputException::~InvalidInputException() throw() {}

const char* RPN::InvalidInputException::what() const throw()
{
    return message.c_str();
}

double RPN::calculate(double a, double b, char c)
{
    if (c == '+')
        return a + b;
    else if (c == '-')
        return a - b;
    else if (c == '*')
        return a * b;
    else
    {
        if (b == 0)
            throw InvalidInputException("Error");
        return a / b;
    }
}

void RPN::calculate(const std::string& expression)
{
    std::stack<double> operands;
    std::istringstream stream(expression);
    std::string token;
    bool empty = true;

    while (stream >> token)
    {
        empty = false;
        if (token.length() != 1)
            throw InvalidInputException("Error");

        char c = token[0];
        if (std::isdigit(static_cast<int>(c)))
        {
            operands.push(c - '0');
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            if (operands.size() < 2)
                throw InvalidInputException("Error");

            double b = operands.top();
            operands.pop();

            double a = operands.top();
            operands.pop();

            operands.push(calculate(a, b, c));
        }
        else
        {
            throw InvalidInputException("Error");
        }
    }

    if (empty || operands.size() != 1)
        throw InvalidInputException("Error");

    std::cout << std::setprecision(15) << operands.top() << std::endl;
}
