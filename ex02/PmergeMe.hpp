#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <vector>
# include <deque>
# include <utility>
# include <iostream>
# include <climits>
# include <cerrno>
# include <cstdlib>

class PmergeMe
{
    private:
        struct Elem
        {
            int value;
            size_t tag;
        };

        std::vector<int> _vec;
        std::deque<int>  _deq;

        void isValidNbr(const std::string &nbr, unsigned long &n);

        static std::vector<size_t> jacobsthalOrder(size_t pairCount);


        template <typename Container>
        size_t lowerBoundInsert(Container &chain, size_t chainSize, Elem value)
        {
            long low = 0;
            long high = static_cast<long>(chainSize) - 1;
            long res = static_cast<long>(chainSize);

            while (low <= high)
            {
                long mid = low + (high - low) / 2;

                if (chain[mid].value >= value.value)
                {
                    high = mid - 1;
                    res = mid;
                }
                else
                {
                    low = mid + 1;
                }
            }
            chain.insert(chain.begin() + res, value);
            return static_cast<size_t>(res);
        }

        template <typename Container>
        void mergeInsertSort(Container &chain, size_t totalCount)
        {
            if (chain.size() <= 1)
                return;

            bool hasOdd = (chain.size() % 2 == 1);
            Elem oddOne;
            if (hasOdd)
                oddOne = chain.back();

            std::vector<std::pair<Elem, Elem> > pairs;
            size_t pairCount = chain.size() / 2;

            for (size_t i = 0; i < pairCount; i++)
            {
                Elem a = chain[2 * i];
                Elem b = chain[2 * i + 1];
                if (a.value >= b.value)
                    pairs.push_back(std::make_pair(a, b));
                else
                    pairs.push_back(std::make_pair(b, a));
            }

            Container bigs;
            for (size_t i = 0; i < pairs.size(); i++)
                bigs.push_back(pairs[i].first);

            mergeInsertSort(bigs, totalCount);

            std::vector<size_t> posOf(totalCount);
            for (size_t k = 0; k < bigs.size(); k++)
                posOf[bigs[k].tag] = k;

            std::vector<size_t> order = jacobsthalOrder(pairs.size());

            for (size_t oi = 0; oi < order.size(); oi++)
            {
                size_t pairIdx = order[oi] - 1;
                size_t partnerTag = pairs[pairIdx].first.tag;
                size_t bound = posOf[partnerTag];

                size_t insPos = lowerBoundInsert(bigs, bound, pairs[pairIdx].second);

                for (size_t j = 0; j < pairs.size(); j++)
                {
                    size_t t = pairs[j].first.tag;
                    if (posOf[t] >= insPos)
                        posOf[t] += 1;
                }
            }

            if (hasOdd)
                lowerBoundInsert(bigs, bigs.size(), oddOne);

            chain = bigs;
        }

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
