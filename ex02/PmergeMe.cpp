#include "PmergeMe.hpp"

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

    if (nbr.empty())
        throw InvalidInputException("Error: Invalid number format.");
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

std::vector<size_t> PmergeMe::jacobsthalOrderVector(size_t pendingCount)
{
    std::vector<size_t> order;
    if (pendingCount == 0)
        return order;

    order.push_back(1);
    if (pendingCount == 1)
        return order;

    std::vector<size_t> jac;
    jac.push_back(0);
    jac.push_back(1);
    while (jac.back() <= pendingCount)
        jac.push_back(jac[jac.size() - 1] + 2 * jac[jac.size() - 2]);

    size_t prev = 1;
    for (size_t k = 3; k < jac.size(); k++)
    {
        size_t t = jac[k];
        size_t upper = (t < pendingCount) ? t : pendingCount;

        for (size_t idx = upper; idx > prev; idx--)
            order.push_back(idx);
        if (upper >= pendingCount)
            break;
        prev = t;
    }
    return order;
}

std::deque<size_t> PmergeMe::jacobsthalOrderDeque(size_t pendingCount)
{
    std::deque<size_t> order;
    if (pendingCount == 0)
        return order;

    order.push_back(1);
    if (pendingCount == 1)
        return order;

    std::deque<size_t> jac;
    jac.push_back(0);
    jac.push_back(1);
    while (jac.back() <= pendingCount)
        jac.push_back(jac[jac.size() - 1] + 2 * jac[jac.size() - 2]);

    size_t prev = 1;
    for (size_t k = 3; k < jac.size(); k++)
    {
        size_t t = jac[k];
        size_t upper = (t < pendingCount) ? t : pendingCount;

        for (size_t idx = upper; idx > prev; idx--)
            order.push_back(idx);
        if (upper >= pendingCount)
            break;
        prev = t;
    }
    return order;
}

size_t PmergeMe::lowerBoundInsertVector(std::vector<Element> &chain,
                    size_t chainSize, Element value)
{
    long low = 0;
    long high = static_cast<long>(chainSize) - 1;
    long result = static_cast<long>(chainSize);

    while (low <= high)
    {
        long mid = low + (high - low) / 2;
        if (chain[mid].value >= value.value)
        {
            result = mid;
            high = mid - 1;
        }
        else
            low = mid + 1;
    }
    chain.insert(chain.begin() + result, value);
    return static_cast<size_t>(result);
}

size_t PmergeMe::lowerBoundInsertDeque(std::deque<Element> &chain,
                    size_t chainSize, Element value)
{
    long low = 0;
    long high = static_cast<long>(chainSize) - 1;
    long result = static_cast<long>(chainSize);

    while (low <= high)
    {
        long mid = low + (high - low) / 2;
        if (chain[mid].value >= value.value)
        {
            result = mid;
            high = mid - 1;
        }
        else
            low = mid + 1;
    }
    chain.insert(chain.begin() + result, value);
    return static_cast<size_t>(result);
}

void PmergeMe::mergeInsertSortVector(std::vector<Element> &chain,
                    size_t totalCount)
{
    if (chain.size() <= 1)
        return;

    bool hasOdd = (chain.size() % 2 == 1);
    Element oddOne;
    if (hasOdd)
        oddOne = chain.back();

    std::vector<std::pair<Element, Element> > pairs;
    size_t pairCount = chain.size() / 2;
    for (size_t i = 0; i < pairCount; i++)
    {
        Element a = chain[2 * i];
        Element b = chain[2 * i + 1];
        if (a.value >= b.value)
            pairs.push_back(std::make_pair(a, b));
        else
            pairs.push_back(std::make_pair(b, a));
    }

    std::vector<Element> bigs;
    for (size_t i = 0; i < pairs.size(); i++)
        bigs.push_back(pairs[i].first);
    mergeInsertSortVector(bigs, totalCount);

    std::vector<size_t> pairOfTag(totalCount);
    for (size_t i = 0; i < pairs.size(); i++)
        pairOfTag[pairs[i].first.tag] = i;

    std::vector<std::pair<Element, Element> > orderedPairs;
    for (size_t i = 0; i < bigs.size(); i++)
        orderedPairs.push_back(pairs[pairOfTag[bigs[i].tag]]);

    std::vector<size_t> posOf(totalCount);
    for (size_t i = 0; i < bigs.size(); i++)
        posOf[bigs[i].tag] = i;

    size_t pendingCount = orderedPairs.size() + (hasOdd ? 1 : 0);
    std::vector<size_t> order = jacobsthalOrderVector(pendingCount);
    for (size_t oi = 0; oi < order.size(); oi++)
    {
        size_t pendingIdx = order[oi] - 1;
        Element value;
        size_t bound;

        if (pendingIdx < orderedPairs.size())
        {
            size_t partnerTag = orderedPairs[pendingIdx].first.tag;
            value = orderedPairs[pendingIdx].second;
            bound = posOf[partnerTag];
        }
        else
        {
            value = oddOne;
            bound = bigs.size();
        }

        size_t insPos = lowerBoundInsertVector(bigs, bound, value);
        for (size_t i = 0; i < orderedPairs.size(); i++)
        {
            size_t tag = orderedPairs[i].first.tag;
            if (posOf[tag] >= insPos)
                posOf[tag]++;
        }
    }
    chain = bigs;
}

void PmergeMe::mergeInsertSortDeque(std::deque<Element> &chain,
                    size_t totalCount)
{
    if (chain.size() <= 1)
        return;

    bool hasOdd = (chain.size() % 2 == 1);
    Element oddOne;
    if (hasOdd)
        oddOne = chain.back();

    std::deque<std::pair<Element, Element> > pairs;
    size_t pairCount = chain.size() / 2;
    for (size_t i = 0; i < pairCount; i++)
    {
        Element a = chain[2 * i];
        Element b = chain[2 * i + 1];
        if (a.value >= b.value)
            pairs.push_back(std::make_pair(a, b));
        else
            pairs.push_back(std::make_pair(b, a));
    }

    std::deque<Element> bigs;
    for (size_t i = 0; i < pairs.size(); i++)
        bigs.push_back(pairs[i].first);
    mergeInsertSortDeque(bigs, totalCount);

    std::deque<size_t> pairOfTag(totalCount);
    for (size_t i = 0; i < pairs.size(); i++)
        pairOfTag[pairs[i].first.tag] = i;

    std::deque<std::pair<Element, Element> > orderedPairs;
    for (size_t i = 0; i < bigs.size(); i++)
        orderedPairs.push_back(pairs[pairOfTag[bigs[i].tag]]);

    std::deque<size_t> posOf(totalCount);
    for (size_t i = 0; i < bigs.size(); i++)
        posOf[bigs[i].tag] = i;

    size_t pendingCount = orderedPairs.size() + (hasOdd ? 1 : 0);
    std::deque<size_t> order = jacobsthalOrderDeque(pendingCount);
    for (size_t oi = 0; oi < order.size(); oi++)
    {
        size_t pendingIdx = order[oi] - 1;
        Element value;
        size_t bound;

        if (pendingIdx < orderedPairs.size())
        {
            size_t partnerTag = orderedPairs[pendingIdx].first.tag;
            value = orderedPairs[pendingIdx].second;
            bound = posOf[partnerTag];
        }
        else
        {
            value = oddOne;
            bound = bigs.size();
        }

        size_t insPos = lowerBoundInsertDeque(bigs, bound, value);
        for (size_t i = 0; i < orderedPairs.size(); i++)
        {
            size_t tag = orderedPairs[i].first.tag;
            if (posOf[tag] >= insPos)
                posOf[tag]++;
        }
    }
    chain = bigs;
}

void PmergeMe::sortVector()
{
    std::vector<Element> chain;
    for (size_t i = 0; i < _vec.size(); i++)
    {
        Element e;
        e.value = _vec[i];
        e.tag = i;
        chain.push_back(e);
    }

    mergeInsertSortVector(chain, chain.size());

    for (size_t i = 0; i < chain.size(); i++)
        _vec[i] = chain[i].value;
}

void PmergeMe::sortDeque()
{
    std::deque<Element> chain;
    for (size_t i = 0; i < _deq.size(); i++)
    {
        Element e;
        e.value = _deq[i];
        e.tag = i;
        chain.push_back(e);
    }

    mergeInsertSortDeque(chain, chain.size());

    for (size_t i = 0; i < chain.size(); i++)
        _deq[i] = chain[i].value;
}

static double calculateTime(const struct timeval &start, const struct timeval &end)
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
    double vecTime = calculateTime(start, end);

    gettimeofday(&start, NULL);
    sortDeque();
    gettimeofday(&end, NULL);
    double deqTime = calculateTime(start, end);

    std::cout << "After: ";
    for (size_t i = 0; i < _vec.size(); i++)
        std::cout << _vec[i] << (i + 1 < _vec.size() ? " " : "");
    std::cout << std::endl;

    std::cout << "Time to process a range of " << _vec.size()
               << " elements with std::vector : " << vecTime << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
               << " elements with std::deque : " << deqTime << " us" << std::endl;
}
