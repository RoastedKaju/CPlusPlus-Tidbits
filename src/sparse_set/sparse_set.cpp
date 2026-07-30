#include <cstdint>
#include <iostream>
#include <vector>
#include <cassert>

constexpr uint32_t SPARSE_NULL = 0xFFFFFFFFu;

// This is a sparse set, sparse[x] -> index into the dense array that contains the actual data
// Initialize this will a NULL value, so we can check which slots are empty
// In ECS this would be our sparse set containing Entity ID's
std::vector<uint32_t> sparse;

// Here lies the actual data, this will be our dense array with no holes
// removal will be done by using the swap method, resulting in O(1) time
std::vector<uint32_t> dense;

size_t size = 0; // number of elements

bool contains(uint32_t value)
{
    if (value >= sparse.size())
    {
        return false;
    }

    const uint32_t index = sparse[value];
    return index < dense.size() && dense[index] == value;
}

void insert(uint32_t value)
{
    assert(value != SPARSE_NULL && "Invalid value, this is reserved for invalid index.");

    // Check if value is already present in set
    if (contains(value))
    {
        return;
    }

    // Grow sparse set, if needed
    if (value >= sparse.size())
    {
        sparse.resize(value + 1, SPARSE_NULL);
    }

    sparse[value] = static_cast<uint32_t>(dense.size());
    dense.push_back(value);
}


void remove(uint32_t value)
{
    if (!contains(value))
    {
        return;
    }

    // Swap-Erase
    const uint32_t index = sparse[value];
    const uint32_t last = dense.back();

    dense[index] = last;
    sparse[last] = index;

    sparse[value] = SPARSE_NULL;
    dense.pop_back();
}

void printSets()
{
    std::cout << "sparse : ";
    for (const auto &element : sparse)
    {
        std::cout << element << " ";
    }
    std::cout << std::endl;
    std::cout << "dense : ";
    for (const auto &element : dense)
    {
        std::cout << element << " ";
    }
}

int main()
{
    insert(0);
    insert(1);
    insert(2);
    insert(3);

    remove(2);

    // assert(contains(2) && "Does not contain");

    printSets();

    return EXIT_SUCCESS;
}