#pragma once

#include <functional>

#include "contomap/editor/Selection.h"

namespace contomap::test::fixtures
{

class SelectionFixture
{
public:
   explicit SelectionFixture(contomap::editor::Selection const &selection);

   void should(std::function<void(contomap::editor::Selection const &)> const &asserter);

private:
   contomap::editor::Selection const &selection;
};

}