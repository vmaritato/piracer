#pragma once
#include <gmpxx.h>
#include <vector>

namespace piracer {
    // NTT/CRT backend for large integer multiplication
    // This is a stub interface for future implementation
    
    // Forward declarations
    struct NTTContext;
    struct CRTContext;
    
    // NTT context for a specific modulus and size
    struct NTTContext {
        mpz_class modulus;
        std::size_t size;
        std::vector<mpz_class> roots_of_unity;
        std::vector<mpz_class> inv_roots_of_unity;
        
        NTTContext(mpz_class mod, std::size_t sz);
        ~NTTContext() = default;
    };
    
    // CRT context for multiple moduli
    struct CRTContext {
        std::vector<mpz_class> moduli;
        std::vector<mpz_class> crt_coeffs;
        
        CRTContext(const std::vector<mpz_class>& mods);
        ~CRTContext() = default;
    };
    
    // Multiply two large integers using NTT/CRT
    // This is the main entry point for the backend
    void mul_ntt(const mpz_class& a, const mpz_class& b, mpz_class& out);
    
    // Alternative: multiply with explicit context
    void mul_ntt_with_context(const mpz_class& a, const mpz_class& b, mpz_class& out,
                             const NTTContext& ntt_ctx, const CRTContext& crt_ctx);
    
    // Utility functions for NTT operations
    void ntt_forward(std::vector<mpz_class>& data, const NTTContext& ctx);
    void ntt_inverse(std::vector<mpz_class>& data, const NTTContext& ctx);
    
    // CRT reconstruction
    mpz_class crt_reconstruct(const std::vector<mpz_class>& residues, const CRTContext& ctx);
    
    // Factory functions for common configurations
    NTTContext create_ntt_context(std::size_t size, const mpz_class& modulus);
    CRTContext create_crt_context(const std::vector<mpz_class>& moduli);
} // namespace piracer 