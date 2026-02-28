template <typename T>
class IHashFamily {
public:
    virtual ~IHashFamily() = default;
    virtual uint64_t hash(T val, uint64_t max_val) = 0;
    virtual void get_new_hash() = 0;
    virtual IHashFamily<T>* clone() const = 0;
};

class StringHashFamily : public IHashFamily<string> {
private:
public:
    std::uint64_t p;
    std::uint64_t z;
    StringHashFamily();
    StringHashFamily(const StringHashFamily& other) {
        this->p = other.p;
        this->z = other.z;
    };
    std::uint64_t hash(std::string key, uint64_t max_val) override;
    void get_new_hash() override;
    IHashFamily<std::string>* clone() const override {
        return new StringHashFamily(*this);
    }
};

StringHashFamily::StringHashFamily() {
    p = 2147483647;
    StringHashFamily::get_new_hash();
}
void StringHashFamily::get_new_hash() {
    z = rand() % p;
}
std::uint64_t StringHashFamily::hash(std::string key, uint64_t max_val) {
    if (max_val == 0) return 0;
    assert(max_val > 0);
    std::uint64_t sum = 0;
    int n = key.length();
    for (int i = 0; i < n; i++) {
        sum = (sum * z + key[i]) % p;
    }
    std::uint64_t ret_val = sum % max_val;
    assert(ret_val < max_val);
    return ret_val;
}
