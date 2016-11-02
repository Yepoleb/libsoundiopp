#ifndef SOUNDIOPP_TEMPLATES_H
#define SOUNDIOPP_TEMPLATES_H

#include <cassert>

template<typename T1, typename T2>
std::vector<T1> array_to_vector(const T2* array_p, size_t array_size)
{
    std::vector<T1> conv_vector(array_size);
    for (size_t i = 0; i < array_size; i++) {
        conv_vector[i] = static_cast<T1>(array_p[i]);
    }
    return conv_vector;
}

template<typename T1, typename T2>
void vector_to_array(const std::vector<T1>& vec, T2* array_p, size_t array_size)
{
    assert(vec.size() <= array_size);
    for (size_t i = 0; i < vec.size(); i++) {
        array_p[i] = static_cast<T2>(vec[i]);
    }
}

#endif //SOUNDIOPP_TEMPLATES_H
