/**
 * @file hcs_random.hpp
 *
 * C++ wrapper class for the hcs_random object.
 */

#ifndef HCS_RANDOM_HPP
#define HCS_RANDOM_HPP

#include <memory>

#include "../libhcs/hcs_random.h"

namespace hcs {

  class random {
  private:
    std::shared_ptr<hcs_random> hr;

  public:
    random() : hr(hcs_init_random(), [](auto& ptr) { hcs_free_random(ptr); }) {}

    int reseed() { return hcs_reseed_random(hr.get()); }

    hcs_random* as_ptr() { return hr.get(); }
  };

}  // namespace hcs

#endif
