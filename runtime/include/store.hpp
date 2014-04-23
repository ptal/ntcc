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

#ifndef NTCC_RUNTIME_STORE_HPP
#define NTCC_RUNTIME_STORE_HPP

#include <gecode/int.hh>
#include <gecode/search.hh>

#include <map>
#include <string>

using namespace Gecode;

class Store : public Space
{
  std::map<std::string, IntVar> constraints;
public:
  Store() = default;

  void entail(std::string x, IntRelType r, std::string y)
  {
    rel(*this, constraints.at(x), r, constraints.at(y));
  }

  void entail(std::string x, IntRelType r, int c)
  {
    rel(*this, constraints.at(x), r, c);
  }

  void new_var(std::string s, int min, int max)
  {
    constraints.emplace(std::make_pair(s, IntVar(*this, min, max)));
  }

  Store(bool share, Store& s) : Space(share, s)
  {
    for(auto &binding : s.constraints)
      constraints[binding.first].update(*this, share, binding.second);
  }

  virtual Space* copy(bool share)
  {
    return new Store(share, *this);
  }

  void branching()
  {
    for(auto& binding: constraints)
      branch(*this, binding.second, INT_VAL_MIN());
  }

  void print() const
  {
    for(const auto& binding: constraints)
      std::cout << binding.first << " = " << binding.second << std::endl;
  }
};

#endif