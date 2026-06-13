#pragma once
#include "../../core/interfaces.h"

class BasicPreprocessor : public IPreprocessor {
public:
    ~BasicPreprocessor() override = default;
    void* preprocess(std::string filename, IStore* store) override;
};
