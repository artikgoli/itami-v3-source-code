#pragma once
template <typename T>
class c_utl_vector {
public:
    auto at(int i) const { return m_data[i]; }
    auto at_ptr(int i) const { return m_data + i; }

    // STL-style iterators
    auto begin() const { return m_data; }
    auto end()   const { return m_data + m_size; }

    int   m_size;
    char  pad0[0x4];
    T* m_data;
    char  pad1[0x8];
};


struct c_utl_vector_alternative {
public:
    int m_count;
private:
    std::byte pad277[0x4];
public:
    uintptr_t m_data;
};


template <typename T>
class c_network_utl_vector_base {
public:
    uint32_t n_size;
    T* p_elements;
};


template <class T, class I, class A>
class c_utl_lean_vector_base {
    using c_allocator = A;

public:
    enum : I {
        external_buffer_marker = (I{ 1 } << (std::numeric_limits<I>::digits - 1))
    };

    T* base() {
        return num_allocated() ? m_p_elements : nullptr;
    }

    int num_allocated() const {
        return (m_n_allocated & (~external_buffer_marker));
    }

    bool is_index_valid(I i) const {
        return (i >= 0) && (i < num_allocated());
    }

protected:
    struct {
        I  m_n_count;
        I  m_n_allocated;
        T* m_p_elements;
    };
};


template <class B, class T, class I>
class c_utl_lean_vector_impl : public B {
    using base_class = B;

public:
    class iterator_t {
    public:
        iterator_t() : m_index(invalid_index()) {}
        iterator_t(I i) : m_index(i) {}

        bool operator==(const iterator_t& it) const { return m_index == it.m_index; }
        bool operator!=(const iterator_t& it) const { return m_index != it.m_index; }

        I m_index;
    };

    T element(int i) {
        return memory->read<T>(
            reinterpret_cast<uintptr_t>(this->base()) + sizeof(T) * i
        );
    }

    T head() {
        return memory->read<T>(
            reinterpret_cast<uintptr_t>(this->base())
        );
    }

    T tail() {
        return memory->read<T>(
            reinterpret_cast<uintptr_t>(this->base()) + sizeof(T) * (this->m_n_count - 1)
        );
    }

    static int invalid_index() {
        return -1;
    }
};


class c_mem_alloc_allocator;

template <class T, class I = short, class A = c_mem_alloc_allocator>
using c_utl_lean_vector =
c_utl_lean_vector_impl<c_utl_lean_vector_base<T, I, A>, T, I>;


template <class T, class I>
struct alignas(8) utl_linked_list_element_t {
    T m_element;
    I m_prev;
    I m_next;
};


template <
    class T,
    class S = unsigned short,
    bool  ML = false,
    class I = S,
    class M = c_utl_lean_vector<utl_linked_list_element_t<T, S>, I>
>
class c_utl_linked_list {
public:
    T element(I index) {
        return internal_element(index).m_element;
    }

    I head() {
        return m_head;
    }

    I next(I index) {
        return internal_element(index).m_next;
    }

    static S invalid_index() {
        return static_cast<S>(M::invalid_index());
    }

protected:
    utl_linked_list_element_t<T, S> internal_element(I index) {
        return memory->read<utl_linked_list_element_t<T, S>>(
            reinterpret_cast<uintptr_t>(m_memory.base()) +
            sizeof(utl_linked_list_element_t<T, S>) * index
        );
    }

    M m_memory;

    I m_head;
    I m_tail;
    I m_first_free;

    typename M::iterator_t m_last_alloc;
    utl_linked_list_element_t<T, S>* m_elements;
};

class c_uniform_random_stream
{
public:
    c_uniform_random_stream() { set_seed(0); }

    void set_seed(int iSeed)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_idum = (iSeed < 0) ? iSeed : -iSeed;
        m_iy = 0;
    }

    int generate_rand_number()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        int j, k;

        if (m_idum <= 0 || !m_iy) {
            if (-(m_idum) < 1)
                m_idum = 1;
            else
                m_idum = -(m_idum);

            for (j = NTAB + 7; j >= 0; --j) {
                k = m_idum / IQ;
                m_idum = IA * (m_idum - k * IQ) - IR * k;
                if (m_idum < 0) m_idum += IM;
                if (j < NTAB) m_iv[j] = m_idum;
            }
            m_iy = m_iv[0];
        }

        k = m_idum / IQ;
        m_idum = IA * (m_idum - k * IQ) - IR * k;
        if (m_idum < 0) m_idum += IM;
        j = m_iy / NDIV;

        if (j < 0) j = 0;
        if (j >= NTAB) j &= NTAB - 1;

        m_iy = m_iv[j];
        m_iv[j] = m_idum;

        return m_iy;
    }

    float random_float(float flLow = 0.0f, float flHigh = 1.0f)
    {
        float fl = AM * static_cast<float>(generate_rand_number());
        if (fl > RNMX) fl = RNMX;
        return (fl * (flHigh - flLow)) + flLow;
    }

private:
    static constexpr int IA = 16807;
    static constexpr int IM = 2147483647;
    static constexpr int IQ = 127773;
    static constexpr int IR = 2836;
    static constexpr int NTAB = 32;
    static constexpr int NDIV = 1 + (IM - 1) / NTAB;
    static constexpr float AM = 1.0f / static_cast<float>(IM);
    static constexpr float EPS = 1.2e-7f;
    static constexpr float RNMX = 1.0f - EPS;

    std::mutex m_mutex;
    int m_idum = 0;
    int m_iy = 0;
    int m_iv[NTAB]{};
};