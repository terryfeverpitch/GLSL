// list::get_allocator
#include <stdlib.h>
#include <iostream>
#include <list>

using namespace std;

int main ()
{
  std::list<int> mylist;
  int * p;

  // allocate an array of 5 elements using mylist's allocator:
  p = mylist.get_allocator().allocate(5, p);

  cout << "list size = " << mylist.size() << endl;
  // assign some values to array
  for (int i=0; i<5; ++i) p[i]=i;

  std::cout << "The allocated array contains:";
  for (int j=0; j<5; ++j) std::cout << ' ' << p[j];
  std::cout << '\n';

  mylist.get_allocator().deallocate(p,0);

  for (int k=0; k<5; ++k) std::cout << ' ' << p[k];
  std::cout << '\n';

  return 0;
}