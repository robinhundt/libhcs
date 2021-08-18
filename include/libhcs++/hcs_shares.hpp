#ifndef HCS_SHARES_HPP
#define HCS_SHARES_HPP

#include <gmpxx.h>
#include "../libhcs/hcs_shares.h"

namespace hcs {

class shares {
private:
    hcs_shares* hs;

public:

    /**
    * Create a new hcs::shares.
    *
    * @param size The number of shares this hcs::shares should store
    * @return A an initialized hcs::shares object.
    */
    shares(unsigned long size): hs(hcs_init_shares(size)) {};

    ~shares() {
        hcs_free_shares(this->hs);
    }


    hcs_shares* as_ptr() {
        return this->hs;
    }

    void set_share(mpz_class &value, unsigned long index) {
        hcs_set_share(this->hs, value.get_mpz_t(), index);
    }

    void set_flag(unsigned long index) {
        hcs_set_flag(this->hs, index);
    }

    void clear_flag(unsigned long index) {
        hcs_clear_flag(this->hs, index);
    }

    int tst_flag(unsigned long index) {
        return hcs_tst_flag(this->hs, index);
    }

};
}



#endif
