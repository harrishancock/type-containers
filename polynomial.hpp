#ifndef TYPE_CONTAINERS_POLYNOMIAL_HPP
#define TYPE_CONTAINERS_POLYNOMIAL_HPP

#include "list.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace polynomial {

template <typename Poly>
using degree = std::integral_constant<typename length<Poly>::value_type,
      0 == length<Poly>::value ? 0 : length<Poly>::value - 1>;

} // namespace polynomial

#endif
