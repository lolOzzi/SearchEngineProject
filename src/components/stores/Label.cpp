#include <cstdint>
#include <string>
//position of Most Significant Bit of 8 bytes in little endian
#define MSB 0x8000000000000000



union Text {
    uintptr_t large;
    char small[8];
};

struct Label {
    Text text{};
    bool is_pointer(){return (text.large & MSB);}

    Label() {
        std::strcpy(text.small, "");
    }

    void set(const char* string) {
        size_t len = std::strlen(string);

        if (is_pointer()) {
            delete [] get();
        }

        if (len > 7) {
            char* newString = new char[len + 1];
            std::strcpy(newString, string);

            uintptr_t ptr = reinterpret_cast<uintptr_t>(newString);
            //set the flag
            text.large = ptr | MSB;

        } else {
            text.large = 0;
            std::strcpy(text.small, string);
        }
    }

    char* get() {
        if (is_pointer()) {
            uintptr_t cleaned = text.large;
            cleaned = cleaned & ~MSB;
            return reinterpret_cast<char*>(cleaned);
        } else {
            return text.small;
        }
    }


    ~Label() {
        if (is_pointer()) {
            uintptr_t cleaned = text.large & ~MSB;
            delete[] reinterpret_cast<char*>(cleaned);
        }
    }



};