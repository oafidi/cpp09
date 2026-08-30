#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange() : inputFile("") {}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const std::string& input) : inputFile(input) {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : inputFile(other.inputFile), exchangeRates(other.exchangeRates) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
    if (this != &other)
    {
        inputFile = other.inputFile;
        exchangeRates = other.exchangeRates;
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

    for (size_t i = 0; i < date.length(); i++)
    {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(static_cast<int>(date[i])))
            throw InvalidFileException("Error: Invalid date format => " + date);
    }

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

void BitcoinExchange::isValidNbr(const std::string &nbr, double &rate, bool maxIntCheck)
{
    bool hasPoint = false;
    bool hasDigit = false;
    size_t i = 0;

    if (nbr.empty())
        throw InvalidFileException("Error: Invalid number format.");
    if (nbr[i] == '-')
        throw InvalidFileException("Error: not a positive number.");
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
        else if (std::isdigit(static_cast<int>(nbr[i])))
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
    rate = std::strtod(nbr.c_str(), NULL);
    if (maxIntCheck && (rate < 0 || rate > INT_MAX))
        throw InvalidFileException("Error: too large a number.");
    else if (!maxIntCheck && (rate < 0 || rate > 1000))
        throw InvalidFileException("Error: too large a number.");
}

void BitcoinExchange::loadExchangeRates()
{
    double rate = 0;

    std::ifstream file("data.csv");
    if (!file.is_open())
    {
        throw InvalidFileException("Error: Could not open exchange rates file data.csv");
    }

    std::string line;
    if (std::getline(file, line))
    {
        size_t commaPos = line.find(',');
        std::string col1 = line.substr(0, commaPos);
        std::string col2 = line.substr(commaPos + 1);
        if (trim(col1) != "date" || trim(col2) != "exchange_rate")
        {
            throw InvalidFileException("Error: Invalid header in exchange rates file data.csv");
        }
    }
    else
    {
        if (file.bad())
            throw InvalidFileException("Error: Could not open exchange rates file data.csv");
        if (file.peek() == -1)
            throw InvalidFileException("Error: exchange rates file data.csv is empty");
    }
    while (std::getline(file, line))
    {
        if (trim(line).empty())
            continue;
        size_t commaPos = line.find(',');
        std::string date = trim(line.substr(0, commaPos));
        std::string rateStr = trim(line.substr(commaPos + 1));
        isValidDate(date);
        isValidNbr(rateStr, rate, true);
        exchangeRates[date] = rate;
    }
}

void BitcoinExchange::processLine(const std::string &line)
{
    double value = 0;
    size_t pipePos = line.find('|');
    if (pipePos == std::string::npos)
    {
        std::cout << "Error: bad input => " << line << std::endl;
        return;
    }

    std::string date = trim(line.substr(0, pipePos));
    std::string valueStr = trim(line.substr(pipePos + 1));
    try
    {
        isValidDate(date);
        isValidNbr(valueStr, value, false);
        std::map<std::string, double>::const_iterator it = exchangeRates.lower_bound(date);
        if (it == exchangeRates.end() || it->first != date)
        {
            if (it == exchangeRates.begin())
            {
                std::cout << "Error: No exchange rate available for date => " << date << std::endl;
                return;
            }
            --it;
        }
        std::cout << date << " => " << value << " = " << (value * it->second) << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void BitcoinExchange::readInputFile()
{
    std::ifstream file(inputFile.c_str());
    if (!file.is_open())
        throw InvalidFileException("Error: could not open file.");

    std::string line;
    if (std::getline(file, line))
    {
        size_t commaPos = line.find('|');
        std::string col1 = line.substr(0, commaPos);
        std::string col2 = line.substr(commaPos + 1);
        if (trim(col1) != "date" || trim(col2) != "value")
        {
            throw InvalidFileException("Error: Invalid header in input file");
        }
    }
    else
    {
        if (file.bad())
            throw InvalidFileException("Error: Could not open input file");
        if (file.peek() == -1)
            throw InvalidFileException("Error: input file is empty");
    }
    while (std::getline(file, line))
        processLine(line);
}

int BitcoinExchange::processInputFile()
{
    try
    {
        loadExchangeRates();
        readInputFile();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}