#pragma once
#include "../../core/interfaces.h"

class BasicPreprocessorHandlesPunctuation : public IPreprocessor {
public:
    ~BasicPreprocessorHandlesPunctuation() override = default;
    void* preprocess(std::string filename, IStore* store) override;
};
