// Copyright 2014 IRCAM (Institut de Recherche et Coordination Acoustique/Musique)
// Copyright 2014 Pierre Talbot <ptalbot@hyc.io>

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "../include/ntcc.hpp"

using namespace Gecode;

// main function
int main(int argc, char* argv[]) 
{
  using constraint = Less<Variable, Constant>;
  using tell = Tell<constraint>;
  using local1 = Local<tell>;
  using local2 = Local<local1>;

  // local(x, y) tell(x < 4)
  // dom(x) = [0..10]
  // dom(y) = [0..10]

  local2 pgm(
    0, FiniteIntegerDomain(0, 10), 
    local1(
      1, FiniteIntegerDomain(0, 10),
      tell(constraint(Variable(0), Constant(4)))));

  Store store;

  pgm(store);

  return 0;
}
