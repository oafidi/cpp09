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

std::string RPN::removeWhitespaces(const std::string& str)
{
    std::string result;

    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isspace(static_cast<unsigned char>(*it)))
            result += *it;
    }

    return result;
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
            throw InvalidInputException("Error: division by zero.");
        return a / b;
    }
}

void RPN::calculate(const std::string& expression)
{
    std::string expr = removeWhitespaces(expression);
    std::stack<double> operands;

    if (expr.empty())
        throw InvalidInputException("Error: Empty expression.");

    for (std::size_t i = 0; i < expr.length(); ++i)
    {
        char c = expr[i];

        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            operands.push(c - '0');
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            if (operands.size() < 2)
            {
                throw InvalidInputException(
                    std::string("Error: not enough operands before '") + c + "'.");
            }

            double b = operands.top();
            operands.pop();

            double a = operands.top();
            operands.pop();

            operands.push(calculate(a, b, c));
        }
        else
        {
            throw InvalidInputException(std::string("Error: invalid character ") + c + ".");
        }
    }

    if (operands.size() != 1)
        throw InvalidInputException("Error: invalid expression.");

    std::cout << operands.top() << std::endl;
}
