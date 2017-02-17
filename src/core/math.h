#ifndef _CG_CORE_MATH_H_
#define _CG_CORE_MATH_H_

namespace cg {

template <typename T, size_t N>
size_t countof(T (&arr)[N]) {
  return std::extent<T[N]>::value;
}

}  // namespace cg

#endif  // _CG_CORE_MATH_H_