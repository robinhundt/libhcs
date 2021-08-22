#ifndef HCS_SHARES_HPP
#define HCS_SHARES_HPP

#include <gmpxx.h>

#include "../libhcs/hcs_shares.h"

namespace hcs {

  class shares {
  private:
    std::shared_ptr<hcs_shares> hs;

  public:
    /**
     * Create a new hcs::shares.
     *
     * @param size The number of shares this hcs::shares should store
     * @return A an initialized hcs::shares object.
     */
    explicit shares(unsigned long size)
        : hs(hcs_init_shares(size), [](auto& ptr) { hcs_free_shares(ptr); }){};

    explicit shares(std::vector<mpz_class>&& shares_vec) : shares(shares_vec.size()) {
      for (std::size_t i = 0; i < shares_vec.size(); ++i) {
        this->set_share(shares_vec[i], i);
      }
    }

    hcs_shares* as_ptr() { return this->hs.get(); }

    void set_share(mpz_class& value, unsigned long index) {
      hcs_set_share(this->hs.get(), value.get_mpz_t(), index);
    }

    void set_flag(unsigned long index) { hcs_set_flag(this->hs.get(), index); }

    void clear_flag(unsigned long index) { hcs_clear_flag(this->hs.get(), index); }

    int tst_flag(unsigned long index) { return hcs_tst_flag(this->hs.get(), index); }
  };
}  // namespace hcs

#endif
