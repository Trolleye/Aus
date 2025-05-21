#pragma once
#include <functional>
#include <vector>
class VectorQuickSort {
public:
    template<typename T, typename comparator>
    static void sort(std::vector<T>& vector, comparator compare)
    {
        if (!vector.empty())
        {
            quick(vector, compare, 0, vector.size() - 1);
        }
    }

    template<typename T, typename comparator>
    static void quick(std::vector<T>& vector, comparator compare, size_t min, size_t max)
    {
        T pivot = vector[min + (max - min) / 2];
        int left = min;
        int right = max;
        do
        {
            while (compare(vector[left], pivot))
            {
                ++left;
            }
            while (right > 0 && compare(pivot, vector[right]))
            {
                --right;
            }
            if (left <= right)
            {
                std::swap(vector[left], vector[right]);
                ++left;
                if (right > 0)
                {
                    --right;
                }
            }
        } while (left <= right);

        if (min < right)
        {
            VectorQuickSort::quick(vector, compare, min, right);
        }
        if (left < max)
        {
            VectorQuickSort::quick(vector, compare, left, max);
        }
    }

};