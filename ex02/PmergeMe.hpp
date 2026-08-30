#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <vector>
# include <deque>
# include <utility>
# include <iostream>
# include <climits>
# include <cerrno>
# include <cstdlib>
# include <sys/time.h>

class PmergeMe
{
    private:
        struct Element
        {
            int value;
            size_t tag;
        };

        std::vector<int> _vec;
        std::deque<int>  _deq;

        void isValidNbr(const std::string &nbr, unsigned long &n);

        static std::vector<size_t> jacobsthalOrderVector(size_t pendingCount);
        static std::deque<size_t> jacobsthalOrderDeque(size_t pendingCount);

        size_t lowerBoundInsertVector(std::vector<Element> &chain,
                    size_t chainSize, Element value);
        size_t lowerBoundInsertDeque(std::deque<Element> &chain,
                    size_t chainSize, Element value);

        void mergeInsertSortVector(std::vector<Element> &chain,
                    size_t totalCount);
        void mergeInsertSortDeque(std::deque<Element> &chain,
                    size_t totalCount);

        void sortVector();
        void sortDeque();

    public:
        PmergeMe();
        ~PmergeMe();
        PmergeMe(const PmergeMe &other);
        PmergeMe &operator=(const PmergeMe &other);

        void run(int ac, char **av);

        class InvalidInputException : public std::exception
        {
            public:
                std::string message;
                InvalidInputException(const std::string& msg);
                ~InvalidInputException() throw();
                const char* what() const throw();
        };
};

#endif
