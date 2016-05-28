#pragma once

#include <algorithm>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

class BasicSerializer
{
    public:

    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
    void Serialize(std::vector<char>& buffer, T number)
    {
        Serialize(buffer, number, sizeof(number));
    }

    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
    void Serialize(std::vector<char>& buffer, T number, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            buffer.push_back(number & 0xff);
            number >>= 8;
        }
    }

    void Serialize(std::vector<char>& buffer, float number)
    {
        Serialize(buffer, *(int32_t*)&number);
    }

    void Serialize(std::vector<char>& buffer, double number)
    {
        Serialize(buffer, *(int64_t*)&number);
    }

    void Serialize(std::vector<char>& buffer, const char * const item)
    {
        std::string str(item);
        Serialize(buffer, str);
    }

    void Serialize(std::vector<char>& buffer, const std::string& item)
    {
        Serialize(buffer, item.size());
        std::copy(item.begin(), item.end(), std::back_inserter(buffer));
    }

    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
    size_t Deserialize(const std::vector<char>& buffer, T& number, size_t index)
    {
        return Deserialize(buffer, number, index, sizeof(number));
    }

    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
    size_t Deserialize(const std::vector<char>& buffer, T& number, size_t index, size_t size)
    {
        if (buffer.size() < index + size)
            return -1;

        memcpy(&number, buffer.data() + index, size);

        return index + size;
    }

    size_t Deserialize(const std::vector<char>& buffer, float& number, size_t index)
    {
        size_t newIndex;
        int32_t numberAsInt;
        newIndex = Deserialize(buffer, numberAsInt, index);
        memcpy(&number, &numberAsInt, sizeof(number));
        return newIndex;
    }

    size_t Deserialize(const std::vector<char>& buffer, double& number, size_t index)
    {
        size_t newIndex;
        int64_t numberAsInt;
        newIndex = Deserialize(buffer, numberAsInt, index);
        memcpy(&number, &numberAsInt, sizeof(number));
        return newIndex;
    }

    size_t Deserialize(const std::vector<char>& buffer, std::string& item, size_t index)
    {
        uint32_t size;
        auto newIndex = Deserialize(buffer, size, index);
        if (buffer.size() <= newIndex + size)
            item.assign(&buffer[newIndex], &buffer[newIndex + size - 1] + 1);
        return newIndex + size;
    }

};