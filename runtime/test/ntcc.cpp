// Copyright 2014 Pierre Talbot (IRCAM)

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
  Store* m = new Store();
  auto x = m->declare(0, 10);
  m->entail(x > 5);
  m->prepare();
  DFS<Store> e(m);
  delete m;
  // search and print all solutions
  while (Store* s = e.next()) {
    s->print(); delete s;
  }
  return 0;
}
