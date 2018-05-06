#ifndef COMMONLIB_NONCOPYABLE_H
#define COMMONLIB_NONCOPYABLE_H

namespace Commonlib {
class CNoncopyable {
protected:
  CNoncopyable() {}
  ~CNoncopyable() {}

private:
  CNoncopyable(const CNoncopyable &);
  CNoncopyable &operator=(const CNoncopyable &);
};
} // namespace Commonlib

#endif // COMMONLIB_NONCOPYABLE_H
