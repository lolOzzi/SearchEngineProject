#pragma once
#include "../../core/interfaces.h"

class BasicPreprocessorHandlesWhitespaceChars : public IPreprocessor {
public:
    ~BasicPreprocessorHandlesWhitespaceChars() override = default;
    void* preprocess(std::string filename, IStore* store) override;

};
