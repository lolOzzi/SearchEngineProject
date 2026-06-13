#pragma once
#include "../../core/interfaces.h"

#include <fstream>
#include <iostream>
class BasicPreprocessorWordCleaner : public IPreprocessor {
public:
    ~BasicPreprocessorWordCleaner() override = default;
    void* preprocess(std::string filename, IStore* store) override;

};
