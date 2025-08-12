#include "piracer/bigmul.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace piracer {
    
    // Simple NTT implementation for power-of-2 sizes
    // This is a basic implementation that can be optimized further
    
    NTTContext::NTTContext(mpz_class mod, std::size_t sz) 
        : modulus(mod), size(sz) {
        // Find primitive root of unity for the modulus
        // For now, use a simple approach with common moduli
        
        // Calculate roots of unity
        mpz_class g = 2;  // Try 2 as generator
        mpz_class phi = modulus - 1;
        
        // Find primitive root
        for (mpz_class test = 2; test < 100; ++test) {
            mpz_class result = 1;
            bool is_primitive = true;
            
            for (mpz_class i = 1; i < phi; ++i) {
                mpz_powm(result.get_mpz_t(), test.get_mpz_t(), i.get_mpz_t(), modulus.get_mpz_t());
                if (result == 1) {
                    is_primitive = false;
                    break;
                }
            }
            
            if (is_primitive) {
                g = test;
                break;
            }
        }
        
        // Calculate roots of unity
        mpz_class w;
        mpz_class exp = phi / size;
        mpz_powm(w.get_mpz_t(), g.get_mpz_t(), exp.get_mpz_t(), modulus.get_mpz_t());
        
        roots_of_unity.resize(size);
        inv_roots_of_unity.resize(size);
        
        mpz_class current = 1;
        for (std::size_t i = 0; i < size; ++i) {
            roots_of_unity[i] = current;
            mpz_invert(inv_roots_of_unity[i].get_mpz_t(), current.get_mpz_t(), modulus.get_mpz_t());
            mpz_mul(current.get_mpz_t(), current.get_mpz_t(), w.get_mpz_t());
            mpz_mod(current.get_mpz_t(), current.get_mpz_t(), modulus.get_mpz_t());
        }
    }
    
    CRTContext::CRTContext(const std::vector<mpz_class>& mods) 
        : moduli(mods) {
        // Calculate CRT coefficients
        mpz_class M = 1;
        for (const auto& m : moduli) {
            M *= m;
        }
        
        crt_coeffs.resize(moduli.size());
        for (std::size_t i = 0; i < moduli.size(); ++i) {
            mpz_class Mi = M / moduli[i];
            mpz_invert(crt_coeffs[i].get_mpz_t(), Mi.get_mpz_t(), moduli[i].get_mpz_t());
            crt_coeffs[i] = (crt_coeffs[i] * Mi) % M;
        }
    }
    
    void mul_ntt(const mpz_class& a, const mpz_class& b, mpz_class& out) {
        // For now, use GMP but prepare for NTT
        // TODO: Implement full NTT multiplication
        out = a * b;
    }
    
    void mul_ntt_with_context(const mpz_class& a, const mpz_class& b, mpz_class& out,
                             const NTTContext& /* ntt_ctx */, const CRTContext& crt_ctx) {
        // Convert to residues for each modulus
        std::vector<mpz_class> a_residues, b_residues;
        
        for (const auto& m : crt_ctx.moduli) {
            a_residues.push_back(a % m);
            b_residues.push_back(b % m);
        }
        
        // Multiply in each residue field using NTT
        std::vector<mpz_class> result_residues;
        for (std::size_t i = 0; i < crt_ctx.moduli.size(); ++i) {
            mpz_class result;
            // Simple multiplication for now, replace with NTT
            result = (a_residues[i] * b_residues[i]) % crt_ctx.moduli[i];
            result_residues.push_back(result);
        }
        
        // Reconstruct using CRT
        out = crt_reconstruct(result_residues, crt_ctx);
    }
    
    void ntt_forward(std::vector<mpz_class>& data, const NTTContext& ctx) {
        // Basic NTT implementation (bit-reversal + butterfly)
        std::size_t n = data.size();
        
        // Bit-reversal permutation
        for (std::size_t i = 1, j = 0; i < n; ++i) {
            std::size_t bit = n >> 1;
            for (; j & bit; bit >>= 1) {
                j ^= bit;
            }
            j ^= bit;
            if (i < j) {
                std::swap(data[i], data[j]);
            }
        }
        
        // NTT butterfly operations
        for (std::size_t len = 2; len <= n; len <<= 1) {
            std::size_t half = len >> 1;
            for (std::size_t i = 0; i < n; i += len) {
                for (std::size_t j = 0; j < half; ++j) {
                    mpz_class u = data[i + j];
                    mpz_class v = (data[i + j + half] * ctx.roots_of_unity[j * (n / len)]) % ctx.modulus;
                    
                    data[i + j] = (u + v) % ctx.modulus;
                    data[i + j + half] = (u - v + ctx.modulus) % ctx.modulus;
                }
            }
        }
    }
    
    void ntt_inverse(std::vector<mpz_class>& data, const NTTContext& ctx) {
        // Inverse NTT
        std::size_t n = data.size();
        
        // Bit-reversal permutation
        for (std::size_t i = 1, j = 0; i < n; ++i) {
            std::size_t bit = n >> 1;
            for (; j & bit; bit >>= 1) {
                j ^= bit;
            }
            j ^= bit;
            if (i < j) {
                std::swap(data[i], data[j]);
            }
        }
        
        // Inverse NTT butterfly operations
        for (std::size_t len = 2; len <= n; len <<= 1) {
            std::size_t half = len >> 1;
            for (std::size_t i = 0; i < n; i += len) {
                for (std::size_t j = 0; j < half; ++j) {
                    mpz_class u = data[i + j];
                    mpz_class v = (data[i + j + half] * ctx.inv_roots_of_unity[j * (n / len)]) % ctx.modulus;
                    
                    data[i + j] = (u + v) % ctx.modulus;
                    data[i + j + half] = (u - v + ctx.modulus) % ctx.modulus;
                }
            }
        }
        
        // Scale by inverse of n
        mpz_class inv_n;
        mpz_invert(inv_n.get_mpz_t(), mpz_class(n).get_mpz_t(), ctx.modulus.get_mpz_t());
        for (auto& x : data) {
            x = (x * inv_n) % ctx.modulus;
        }
    }
    
    mpz_class crt_reconstruct(const std::vector<mpz_class>& residues, const CRTContext& ctx) {
        mpz_class result = 0;
        for (std::size_t i = 0; i < residues.size(); ++i) {
            result = (result + residues[i] * ctx.crt_coeffs[i]) % ctx.moduli[i];
        }
        return result;
    }
    
    NTTContext create_ntt_context(std::size_t size, const mpz_class& modulus) {
        return NTTContext(modulus, size);
    }
    
    CRTContext create_crt_context(const std::vector<mpz_class>& moduli) {
        return CRTContext(moduli);
    }
    
} // namespace piracer 