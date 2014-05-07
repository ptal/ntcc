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
int main(int argc, char* argv[]) {
  // create model and search engine
  Store store;
  auto x = store.declare(0, 10);
  auto y = store.declare(5, 5);
  store.entail(store[x] > store[y]);

  if(store.ask(store[x] >= 5))
    std::cout << "x >= 5";
  else
    std::cout << "x < 5";
  std::cout << std::endl;
  
  if(store.ask(store[x] < 5))
    std::cout << "x < 5";
  else
    std::cout << "x >= 5";
  std::cout << std::endl;



  store.print();
  // typedef Tell<Less<Variable<0>, Constant<7>>> ProgramSource;

  // typedef Compile<ProgramSource> Program;

  return 0;
}
