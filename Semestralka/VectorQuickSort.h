#pragma once
#include <functional>
#include <vector>
class VectorQuickSort {
public:
    template<typename T, typename comparator>
    static void sort(std::vector<T>& vector, comparator compare)
    {
        if (!vector.isEmpty())
        {
            quick(vector, compare, 0, vector.size() - 1);
        }
    }

    template<typename T, typename comparator>
    void quick(std::vector<T>& vector, comparator compare, size_t min, size_t max)
    {
        T pivot = vector.access(min + (max - min) / 2)->data_;
        int left = min;
        int right = max;
        do
        {
            while (compare(vector.access(left)->data_, pivot))
            {
                ++left;
            }
            while (right > 0 && compare(pivot, vector.access(right)->data_))
            {
                --right;
            }
            if (left <= right)
            {
                std::swap(vector.access(left)->data_, vector.access(right)->data_);
                ++left;
                if (right > 0)
                {
                    --right;
                }
            }
        } while (left <= right);

        if (min < right)
        {
            quick(vector, compare, min, right);
        }
        if (left < max)
        {
            quick(vector, compare, left, max);
        }
    }

};