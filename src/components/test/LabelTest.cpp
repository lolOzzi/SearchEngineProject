#include "../stores/RadixTree.cpp"

int main(int argc, char* argv[]) {
  Label pointer = Label();
  pointer.set("pointerpointer");
  assert(pointer.is_pointer());

  Label charArray = Label();
  charArray.set("char");
  assert(!charArray.is_pointer());
}


