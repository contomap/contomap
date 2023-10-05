#include <gmock/gmock.h>

#include "SelectionFixture.h"

using contomap::editor::Selection;

using contomap::test::fixtures::SelectionFixture;

SelectionFixture::SelectionFixture(Selection const &selection)
   : selection(selection)
{
}

void SelectionFixture::should(std::function<void(Selection const &)> const &asserter)
{
   asserter(selection);
}
