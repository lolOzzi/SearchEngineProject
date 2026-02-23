

template <typename T, typename U>
class GenericCollisionFree<T, U> {
private:
    int n;
    int size;
    int arr_size;
    T* arr;
    void do_new(int old_arr_size);
public:
    ~GenericCollisionFree() = default;
    GenericCollisionFree(int size);
    U* add(T key, U val);
    U* get(T key);
};




template <typename T, typename U>
class GenericFKSDictionary<T, U> {
private:
    int num_buckets;


public:
    void add(T key, U val);
    U* get(T key);

};