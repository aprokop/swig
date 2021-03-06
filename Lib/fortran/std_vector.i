//---------------------------------*-SWIG-*----------------------------------//
/*!
 * \file   fortran/std_vector.i
 * \author Seth R Johnson
 * \date   Mon Dec  5 09:18:06 2016
 * \brief  Fortran std::vector code
 * \note   Copyright (c) 2016 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

%{
#include <vector>
%}

%include "std_common.i"

namespace std
{

template<class _Tp, class _Alloc = std::allocator< _Tp > >
class vector
{
  public:
    typedef std::size_t       size_type;
    typedef _Tp               value_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef _Tp&              reference;
    typedef const _Tp&        const_reference;
    typedef _Alloc            allocator_type;

  public:

    // Constructors
    vector();
    vector(size_type count);
    vector(size_type count, const value_type& v);

    // Accessors
    size_type size() const;
    size_type capacity() const;
    bool empty() const;
    void clear();

    // Modify
    void reserve(size_type count);
    void resize(size_type count);
    void resize(size_type count, const value_type& v);
    void push_back(const value_type& v);

    const_reference front() const;
    const_reference back() const;

    // Extend for Fortran
%extend {
    // C indexing used here!
    void set(size_type index, const_reference v)
    {
        // TODO: check range
        (*$self)[index] = v;
    }

    // C indexing used here!
    value_type get(size_type index)
    {
        // TODO: check range
        return (*$self)[index];
    }
} // end extend

};

// Specialize on bool
template<class _Alloc >
class vector<bool,_Alloc >
{
    /* NOT IMPLEMENTED */
};

} // end namespace std

//---------------------------------------------------------------------------//
// end of fortran/std_vector.i
//---------------------------------------------------------------------------//
