#include <utility>

#include "contomap/editor/VersionInfo.h"

using contomap::editor::VersionInfo;

VersionInfo::VersionInfo(std::string branch, std::string revision, std::string tag)
   : branch(std::move(branch))
   , revision(std::move(revision))
   , tag(std::move(tag))
{
}

std::string VersionInfo::humanReadable() const
{
   return tag.empty() ? (branch + "@" + revision) : tag;
}
