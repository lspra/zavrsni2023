#include "records.hpp"

array::array(std::string name_, size_t dimension, std::vector<size_t> size_) {
    this->name = name_;
    if(dimension > size_.size())
        dimension = size_.size();
    for(size_t i = 0; i < dimension; i++)
        this->size[i] = size_[i];
}
