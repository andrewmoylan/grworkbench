#pragma once

#include "lax/lax.hpp"
#include <map>
#include <string>

namespace grwb
{
  namespace lax_integration
  {
    namespace builtins
    {
      using lax_interpreter::lax;
      using namespace std;

      const map<string, lax>& builtins();
    }
  }
}
