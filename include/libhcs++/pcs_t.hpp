/**
 * @file pcs_t.h
 *
 * The threshold Paillier scheme offers the same properties as the Paillier
 * scheme, with the extra security that decryption is split performed between a
 * number of parties instead of just a single trusted party. It is much more
 * complex to set up a system which provides this, so determine if you actually
 * require this before using.
 *
 * All mpz_t values can be aliases unless otherwise stated.
 *
 * \warning All indexing for the servers and polynomial functions should be
 * zero-indexed, as is usual when working with c arrays. The functions
 * themselves correct for this internally, and 1-indexing servers may result
 * in incorrect results.
 */

#ifndef HCS_PCS_T_HPP
#define HCS_PCS_T_HPP

#include <vector>
#include <gmpxx.h>
#include <memory>
#include "../libhcs/pcs_t.h"
#include "shares.hpp"
#include "random.hpp"

namespace hcs::pcs_t {

class public_key {

private:
    std::shared_ptr<pcs_t_public_key> pk;
    hcs::random hr;

public:
    explicit public_key(hcs::random &hr): pk(pcs_t_init_public_key(), [](auto& ptr){ pcs_t_free_public_key(ptr);}), hr(hr) {}

    pcs_t_public_key* as_ptr() {
        return pk.get();
    }

    hcs::random get_rand() {
        return hr;
    }

    /* Encryption functions acting on a key */
    mpz_class encrypt(mpz_class &op) {
        mpz_class rop;
        pcs_t_encrypt(pk.get(), hr.as_ptr(), rop.get_mpz_t(), op.get_mpz_t());
        return rop;
    }

    mpz_class reencrypt(mpz_class &op) {
        mpz_class rop;
        pcs_t_reencrypt(pk.get(), hr.as_ptr(), rop.get_mpz_t(), op.get_mpz_t());
        return rop;
    }

    mpz_class ep_add(mpz_class &c1, mpz_class &c2) {
        mpz_class rop;
        pcs_t_ep_add(pk.get(), rop.get_mpz_t(), c1.get_mpz_t(), c2.get_mpz_t());
        return rop;
    }

    mpz_class ee_add(mpz_class &c1, mpz_class &c2) {
        mpz_class rop;
        pcs_t_ee_add(pk.get(), rop.get_mpz_t(), c1.get_mpz_t(), c2.get_mpz_t());
        return rop;
    }

    mpz_class ep_mul(mpz_class &c1, mpz_class &p1) {
        mpz_class rop;
        pcs_t_ep_mul(pk.get(), rop.get_mpz_t(), c1.get_mpz_t(), p1.get_mpz_t());
        return rop;
    }

    // May want to make a new object which holds shares, given there are a
    // number of specific operations that are useful to do on them, plus
    // they require a set size and we can enforce that through some function
    mpz_class share_combine(hcs::shares &shares) {
        mpz_class rop;
        pcs_t_share_combine(pk.get(), rop.get_mpz_t(), shares.as_ptr());
        return rop;
    }

    void clear() {
        pcs_t_clear_public_key(pk.get());
    }
};

class private_key {

private:
    std::shared_ptr<pcs_t_private_key> vk;
    hcs::random hr;

public:
    explicit private_key(hcs::random &hr): vk(pcs_t_init_private_key(), [](auto& ptr) { pcs_t_free_private_key(ptr);}), hr(hr) {}

//    // delete copy constructor because it can result in double free
//    private_key(const private_key&) = delete;
//
//    private_key(private_key&& other) noexcept : vk(other.vk), hr(other.hr) {
//        other.vk = nullptr;
//        other.hr = nullptr;
//    }

//    ~private_key() {
//        if (vk != nullptr) {
//            pcs_t_free_private_key(vk);
//            hr->dec_refcount();
//        }
//    }

    pcs_t_private_key* as_ptr() {
        return vk.get();
    }

    hcs::random get_rand() const {
        return hr;
    }

    void clear() {
        pcs_t_clear_private_key(vk.get());
    }
};

class polynomial {

private:
    hcs::random hr;
    std::shared_ptr<pcs_t_polynomial> px;

public:
    explicit polynomial(private_key& pk): hr(pk.get_rand()), px(pcs_t_init_polynomial(pk.as_ptr(), hr.as_ptr()), [](auto& ptr) { pcs_t_free_polynomial(ptr);}) {}

//    // delete copy constructor because it can result in double free
//    polynomial(const polynomial&) = delete;
//
//    polynomial(polynomial&& other) noexcept : px(other.px), hr(other.hr) {
//        other.px = nullptr;
//        other.hr = nullptr;
//    }
//
//    ~polynomial() {
//        if (px != nullptr) {
//            pcs_t_free_polynomial(px);
//            hr->dec_refcount();
//        }
//    }

    pcs_t_polynomial* as_ptr() {
        return px.get();
    }

    hcs::random get_rand() {
        return hr;
    }

    mpz_class compute(pcs_t::private_key &vk, const unsigned long x) {
        mpz_class rop;
        pcs_t_compute_polynomial(vk.as_ptr(), px.get(), rop.get_mpz_t(), x);
        return rop;
    }
};

class auth_server {

private:
    std::shared_ptr<pcs_t_auth_server> au;

public:
    auth_server(mpz_class &op, unsigned long id): au(pcs_t_init_auth_server(), [](auto& ptr) { pcs_t_free_auth_server(ptr);}) {
        pcs_t_set_auth_server(au.get(), op.get_mpz_t(), id);
    }

    // delete copy constructor because it can result in double free
//    auth_server(const auth_server&) = delete;
//
//    auth_server(auth_server&& other) noexcept : au(other.au) {
//        other.au = nullptr;
//    }
//
//    ~auth_server() {
//        if (au != nullptr) {
//            pcs_t_free_auth_server(au);
//        }
//    }

    pcs_t_auth_server *as_ptr() {
        return au.get();
    }

    mpz_class share_decrypt(public_key &pk, mpz_class &cipher1) {
        mpz_class rop;
        pcs_t_share_decrypt(pk.as_ptr(), au.get(), rop.get_mpz_t(),
                cipher1.get_mpz_t());
        return rop;
    }
};

inline void generate_key_pair(public_key &pk, private_key &vk,
        const unsigned long bits, const unsigned long l, const unsigned long w)
{
    pcs_t_generate_key_pair(pk.as_ptr(), vk.as_ptr(), vk.get_rand().as_ptr(),
                            bits, w, l);
}

inline int verify_key_pair(public_key &pk, private_key &vk) {
    return pcs_t_verify_key_pair(pk.as_ptr(), vk.as_ptr());
}

}

#endif
