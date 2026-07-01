#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange() : inputFile("") {}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const std::string& input) : inputFile(input) {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : inputFile(other.inputFile) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
    if (this != &other)
    {
        inputFile = other.inputFile;
    }
    return *this;
}

BitcoinExchange::InvalidFileException::InvalidFileException(const std::string& msg) : message(msg) {}

BitcoinExchange::InvalidFileException::~InvalidFileException() throw() {}
const char* BitcoinExchange::InvalidFileException::what() const throw()
{
    return message.c_str();
}

std::string BitcoinExchange::trim(const std::string &str)
{
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t start = str.find_first_not_of(WHITESPACE);
    size_t end = str.find_last_not_of(WHITESPACE);

    if (start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

void BitcoinExchange::isValidDate(const std::string &date)
{
    if (date.length() != 10 || date[4] != '-' || date[7] != '-')
        throw InvalidFileException("Error: Invalid date format => " + date);

    int year = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day = std::atoi(date.substr(8, 2).c_str());

    if (year < 2009 || month < 1 || month > 12 || day < 1 || day > 31)
        throw InvalidFileException("Error: Invalid date => " + date);

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        throw InvalidFileException("Error: Invalid date => " + date);
    if (month == 2)
    {
        bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if ((isLeapYear && day > 29) || (!isLeapYear && day > 28))
            throw InvalidFileException("Error: Invalid date => " + date);
    }
}

void BitcoinExchange::isValidNbr(const std::string &nbr, bool maxIntCheck)
{
    bool hasPoint = false;
    bool hasDigit = false;
    double value = 0.0;
    size_t i = 0;

    if (nbr[i] == '-')
        throw InvalidFileException("Error: not positive number.");
    if (nbr[i] == '+')
        i++;
    for (; i < nbr.length(); i++)
    {
        if (nbr[i] == '.')
        {
            if (hasPoint)
                throw InvalidFileException("Error: Invalid number format => " + nbr);
            hasPoint = true;
        }
        else if (std::isdigit(nbr[i]))
        {
            hasDigit = true;
        }
        else
        {
            throw InvalidFileException("Error: Invalid number format => " + nbr);
        }
    }
    if (!hasDigit)
        throw InvalidFileException("Error: Invalid number format.");
    value = std::strtod(nbr.c_str(), NULL);
    if (maxIntCheck && (value < 0 || value > INT_MAX))
        throw InvalidFileException("Error: too large a number.");
    else if (!maxIntCheck && (value < 0 || value > 1000))
        throw InvalidFileException("Error: too large a number.");
}

void BitcoinExchange::loadExchangeRates()
{
    std::ifstream file("data.csv");
    if (!file.is_open())
    {
        throw InvalidFileException("Error: Could not open exchange rates file data.csv");
    }

    std::string line;
    if (std::getline(file, line))
    {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos)
        {
            std::string col1 = line.substr(0, commaPos);
            std::string col2 = line.substr(commaPos + 1);
            if (trim(col1) != "date" || trim(col2) != "exchange_rate")
            {
                throw InvalidFileException("Error: Invalid header in exchange rates file data.csv");
            }
        }
    }
    while (std::getline(file, line))
    {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos)
        {
            std::string date = trim(line.substr(0, commaPos));
            std::string rateStr = trim(line.substr(commaPos + 1));
            isValidDate(date);
            isValidNbr(rateStr, true);
            double rate = std::strtod(rateStr.c_str(), NULL);
            exchangeRates[date] = rate;
        }
    }
}


void BitcoinExchange::processInputFile()
{
    std::ifstream file(inputFile.c_str());
    std::string line;
    if (!file.is_open())
        std::cerr << "Error: Could not open input file " << inputFile << std::endl;
    try
    {
        loadExchangeRates();
    }
    catch (const InvalidFileException& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }

    if (std::getline(file, line))
    {
        size_t pipePos = line.find('|');
        if (pipePos != std::string::npos)
        {
            std::string col1 = line.substr(0, pipePos);
            std::string col2 = line.substr(pipePos + 1);
            if (trim(col1) != "date" || trim(col2) != "value")
            {
                std::cerr << "Error: Invalid header in input file " << inputFile << std::endl;
                return;
            }
        }
    }
    while (std::getline(file, line))
    {
        size_t pipePos = line.find('|');
        if (pipePos != std::string::npos)
        {
            std::string date = trim(line.substr(0, pipePos));
            std::string valueStr = trim(line.substr(pipePos + 1));
            try
            {
                isValidDate(date);
                isValidNbr(valueStr, false);
                double value = std::strtod(valueStr.c_str(), NULL);
                if (exchangeRates.find(date) != exchangeRates.end())
                {
                    double rate = exchangeRates[date];
                    std::cout << date << " => " << value << " = " << (value * rate) << std::endl;
                }
                else
                {
                    std::map<std::string, double>::const_iterator it = exchangeRates.lower_bound(date);
                    if (it == exchangeRates.begin())
                        std::cout << "Error: No exchange rate available for date => " << date << std::endl;
                    else
                    {
                        --it;
                        double rate = it->second;
                        std::cout << date << " => " << value << " = " << (value * rate) << std::endl;
                    }
                }
            }
            catch (std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "Error: bad input => " << line << std::endl;
        }
    }
}