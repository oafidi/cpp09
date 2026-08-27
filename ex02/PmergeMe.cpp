#include "PmergeMe.hpp"
#include <sys/time.h>

PmergeMe::PmergeMe() {}

PmergeMe::~PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &other) : _vec(other._vec), _deq(other._deq) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
    if (this != &other)
    {
        _vec = other._vec;
        _deq = other._deq;
    }
    return *this;
}

PmergeMe::InvalidInputException::InvalidInputException(const std::string& msg) : message(msg) {}

PmergeMe::InvalidInputException::~InvalidInputException() throw() {}

const char* PmergeMe::InvalidInputException::what() const throw()
{
    return message.c_str();
}

void PmergeMe::isValidNbr(const std::string &nbr, unsigned long &n)
{
    bool hasDigit = false;
    size_t i = 0;

    if (nbr[i] == '-')
        throw InvalidInputException("Error: not a positive number.");
    if (nbr[i] == '+')
        i++;
    for (; i < nbr.length(); i++)
    {
        if (std::isdigit(static_cast<int>(nbr[i])))
        {
            hasDigit = true;
        }
        else
        {
            throw InvalidInputException("Error: Invalid number format => " + nbr);
        }
    }
    if (!hasDigit)
        throw InvalidInputException("Error: Invalid number format.");

    errno = 0;
    n = std::strtoul(nbr.c_str(), NULL, 10);
    if (errno == ERANGE || n > static_cast<unsigned long>(INT_MAX))
        throw InvalidInputException("Error: too large a number.");
}

std::vector<size_t> PmergeMe::jacobsthalOrder(size_t pairCount)
{
    std::vector<size_t> order;
    if (pairCount == 0)
        return order;

    order.push_back(1);
    if (pairCount == 1)
        return order;

    std::vector<size_t> jac;
    jac.push_back(0);
    jac.push_back(1);
    while (jac.back() <= pairCount)
        jac.push_back(jac[jac.size() - 1] + 2 * jac[jac.size() - 2]);

    size_t prev = 1;
    for (size_t k = 3; k < jac.size(); k++)
    {
        size_t t = jac[k];
        size_t upper = (t < pairCount) ? t : pairCount;

        for (size_t idx = upper; idx > prev; idx--)
            order.push_back(idx);
        if (upper >= pairCount)
            break;
        prev = t;
    }
    return order;
}

void PmergeMe::sortVector()
{
    std::vector<Elem> chain;
    for (size_t i = 0; i < _vec.size(); i++)
    {
        Elem e;
        e.value = _vec[i];
        e.tag = i;
        chain.push_back(e);
    }

    mergeInsertSort(chain, chain.size());

    for (size_t i = 0; i < chain.size(); i++)
        _vec[i] = chain[i].value;
}

void PmergeMe::sortDeque()
{
    std::deque<Elem> chain;
    for (size_t i = 0; i < _deq.size(); i++)
    {
        Elem e;
        e.value = _deq[i];
        e.tag = i;
        chain.push_back(e);
    }

    mergeInsertSort(chain, chain.size());

    for (size_t i = 0; i < chain.size(); i++)
        _deq[i] = chain[i].value;
}

static double elapsedMicroseconds(const struct timeval &start, const struct timeval &end)
{
    double sec = static_cast<double>(end.tv_sec - start.tv_sec) * 1000000.0;
    double usec = static_cast<double>(end.tv_usec - start.tv_usec);
    return sec + usec;
}

void PmergeMe::run(int ac, char **av)
{
    unsigned long n;

    for (int i = 1; i < ac; i++)
    {
        isValidNbr(av[i], n);
        _vec.push_back(static_cast<int>(n));
        _deq.push_back(static_cast<int>(n));
    }

    std::cout << "Before: ";
    for (size_t i = 0; i < _vec.size(); i++)
        std::cout << _vec[i] << (i + 1 < _vec.size() ? " " : "");
    std::cout << std::endl;

    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);
    sortVector();
    gettimeofday(&end, NULL);
    double vecTime = elapsedMicroseconds(start, end);

    gettimeofday(&start, NULL);
    sortDeque();
    gettimeofday(&end, NULL);
    double deqTime = elapsedMicroseconds(start, end);

    std::cout << "After: ";
    for (size_t i = 0; i < _vec.size(); i++)
        std::cout << _vec[i] << (i + 1 < _vec.size() ? " " : "");
    std::cout << std::endl;

    std::cout << "Time to process a range of " << _vec.size()
               << " elements with std::vector : " << vecTime << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
               << " elements with std::deque : " << deqTime << " us" << std::endl;
}
