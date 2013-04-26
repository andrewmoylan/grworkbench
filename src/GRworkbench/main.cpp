#include <exception>
#include <iostream>
#include "application.hpp"

using std::endl;
using std::cerr;
using std::string;

int main(int, char **) try
{
  return grwb::application::execute();
}
catch (const std::exception& x)
{
  cerr << "exception[" << typeid(x).name() << "]: " << x.what() << endl;
  return EXIT_FAILURE;
}
catch (const string& s)
{
  cerr << "exception[string]: " << s << endl;
  return EXIT_FAILURE;
}
catch (const char* s)
{
  cerr << "exception[char*]: " << s << endl;
  return EXIT_FAILURE;
}
catch (...)
{
  cerr << "exception[?]:" << endl;
  return EXIT_FAILURE;
}
