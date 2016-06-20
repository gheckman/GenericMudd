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
    void Serialize(std::vector<char>& buffer, T number, int size)
    {
        for (int i = 0; i < size; ++i)
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

    void Serialize(std::vector<char>& buffer, const std::string& item, bool storeSize)
    {
        if (storeSize)
            Serialize(buffer, item.size());
        std::copy(item.begin(), item.end(), std::back_inserter(buffer));
    }

    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
    int Deserialize(const std::vector<char>& buffer, T& number, int index)
    {
        return Deserialize(buffer, number, index, sizeof(number));
    }

    template <
        typename T,
        typename = typename std::enable_if<std::is_integral<T>::value, T>::type
    >
    int Deserialize(const std::vector<char>& buffer, T& number, int index, size_t size)
    {
        if (buffer.size() < index + size)
            return -1;

        memcpy(&number, buffer.data() + index, size);

        return index + size;
    }

    int Deserialize(const std::vector<char>& buffer, float& number, int index)
    {
        int32_t numberAsInt;
        index = Deserialize(buffer, numberAsInt, index);
        memcpy(&number, &numberAsInt, sizeof(number));
        return index;
    }

    int Deserialize(const std::vector<char>& buffer, double& number, int index)
    {
        int64_t numberAsInt;
        index = Deserialize(buffer, numberAsInt, index);
        memcpy(&number, &numberAsInt, sizeof(number));
        return index;
    }

    int Deserialize(const std::vector<char>& buffer, std::string& item, int index)
    {
        size_t size;
        auto newIndex = Deserialize(buffer, size, index);
        return Deserialize(buffer, item, newIndex, size);
    }

    int Deserialize(const std::vector<char>& buffer, std::string& item, int index, size_t size)
    {
        if (buffer.size() <= index + size)
            item.assign(&buffer[index], &buffer[index + size - 1] + 1);
        return index + size;
    }

};