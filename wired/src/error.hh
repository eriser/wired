#ifndef ERROR_HH_
# define ERROR_HH_

#include <string>

namespace Error
{
  class File
  {
  private:
    std::string name;
    std::string sup;
  public:
    File(std::string s1, std::string s2) : name(s1), sup(s2) { }
    std::string getName() { return name; }
    std::string getSup() { return sup; }
  };

  class NoChannels {};
};

#endif /* ERROR_HH_ */
