#pragma once

#include <string>

namespace contomap::editor
{

/**
 * VersionInfo describes the origin of the source.
 * It is based on source control management information.
 */
class VersionInfo
{
public:
   /**
    * The global version information of the application. The state of this value
    * will be determined during build.
    */
   static VersionInfo const GLOBAL;

   /**
    * Constructor.
    *
    * @param branch the name of the branch
    * @param revision the commit identifier of the state
    * @param tag the label text of a particular commit
    */
   VersionInfo(std::string branch, std::string revision, std::string tag);

   /**
    * @return a string meant for human interpretation
    */
   [[nodiscard]] std::string humanReadable() const;

private:
   std::string branch;
   std::string revision;
   std::string tag;
};

} // namespace contomap::editor
