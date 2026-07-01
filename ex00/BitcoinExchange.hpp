#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <iostream>
# include <fstream>
# include <climits>
# include <cstdlib>
# include <map>

class BitcoinExchange
{
    private:
        std::string inputFile;
        std::map<std::string, double> exchangeRates;
        void loadExchangeRates();
        std::string trim(const std::string &str);
        void isValidDate(const std::string &date);
        void isValidNbr(const std::string &nbr, bool maxIntCheck);

    public:
        BitcoinExchange();
        ~BitcoinExchange();
        BitcoinExchange(const std::string& input);
        BitcoinExchange(const BitcoinExchange& other);
        BitcoinExchange& operator=(const BitcoinExchange& other);

        class InvalidFileException : public std::exception
        {
            public:
                std::string message;
                InvalidFileException(const std::string& msg);
                ~InvalidFileException() throw();
                const char* what() const throw();
        };

        void processInputFile();
};

#endif