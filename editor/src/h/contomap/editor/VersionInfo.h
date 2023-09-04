#pragma once

#include <string>

namespace contomap::editor
{

class VersionInfo
{
public:
   static VersionInfo const GLOBAL;

   VersionInfo(std::string branch, std::string revision, std::string tag);

   [[nodiscard]] std::string humanReadable() const;

private:
   std::string branch;
   std::string revision;
   std::string tag;
};

} // namespace contomap::editor
