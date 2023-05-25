#include "records.hpp"

array::array(Token* token_, size_t dimension, std::vector<size_t> size_) {
    this->token = token_;
    if(dimension > size_.size())
        dimension = size_.size();
    for(size_t i = 0; i < dimension; i++)
        this->size[i] = size_[i];
}
