#include <iostream>
#include <limits>
#include <vector>

using Handle = std::uint64_t;
using HandleIndex = std::uint32_t;
using HandleGeneration = std::uint32_t;

class HandleManager
{
public:
    static constexpr Handle invalid_handle = std::numeric_limits<Handle>::max();

    HandleManager() = default;
    ~HandleManager() = default;

    HandleManager(const HandleManager &) = delete;
    HandleManager &operator=(const HandleManager &) = delete;

    static Handle MakeHandle(HandleGeneration generation, HandleIndex index)
    {
        return (static_cast<Handle>(generation) << 32) | index;
    }

    static HandleGeneration GetGeneration(Handle handle)
    {
        return static_cast<HandleGeneration>(handle >> 32);
    }

    static HandleIndex GetIndex(Handle handle)
    {
        return static_cast<HandleIndex>(handle);
    }

    bool IsValid(Handle handle) const
    {
        if (handle == invalid_handle)
        {
            return false;
        }

        const auto index = GetIndex(handle);
        // check if index is within bounds
        if (index >= handles_.size())
        {
            return false;
        }
        return handles_[index] == handle;
    }

    Handle AddHandle()
    {
        // check free list first
        if (!free_list_.empty())
        {
            std::fprintf(stdout, "Reusing handle from free list\n");
            const auto handle = free_list_.back();
            free_list_.pop_back();
            return handle;
        }

        if (handles_.size() > std::numeric_limits<HandleIndex>::max())
        {
            return invalid_handle;
        }

        const auto idx = static_cast<std::uint32_t>(handles_.size());
        handles_.push_back(MakeHandle(0, idx));
        return handles_.back();
    }

    void RemoveHandle(Handle handle)
    {
        if (!IsValid(handle))
        {
            return;
        }

        const auto generation = GetGeneration(handle);
        const auto index = GetIndex(handle);

        // increment generation and update handle
        handles_[index] = MakeHandle(generation + 1, index);

        // add to free list
        free_list_.push_back(handles_[index]);
    }

private:
    std::vector<Handle> handles_;
    std::vector<Handle> free_list_;
};

int main()
{
    HandleManager manager{};

    const auto someHandle = manager.AddHandle();

    std::fprintf(stdout, "Handle: %llu\n", someHandle);

    manager.RemoveHandle(someHandle);

    const auto reusedHandle = manager.AddHandle();

    std::fprintf(stdout, "Handle: %llu\n", reusedHandle);

    if (manager.IsValid(reusedHandle))
    {
        std::cout << "VALID at index " << HandleManager::GetIndex(reusedHandle) << std::endl;
    }

    HandleManager::GetGeneration(reusedHandle);
    HandleManager::GetIndex(reusedHandle);
}
