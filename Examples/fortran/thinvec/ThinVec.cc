//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   thinvec/ThinVec.cc
 * \author Seth R Johnson
 * \date   Fri Dec 02 15:55:20 2016
 * \brief  ThinVec class definitions.
 * \note   Copyright (c) 2016 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include "ThinVec.hh"

#include <cassert>
#include <iostream>
using std::cout;
using std::endl;

//---------------------------------------------------------------------------//
// TEMPLATED METHODS
//---------------------------------------------------------------------------//
template<typename T>
void ThinVec<T>::assign(const_view_type arr)
{
    cout << "assigning " << arr.second << " from " << arr.first
        << " to " << this << endl;
    d_data.assign(arr.first, arr.first + arr.second);
}

template<typename T>
typename ThinVec<T>::view_type ThinVec<T>::view()
{
    if (empty())
        return view_type(NULL, 0);
    return view_type(&d_data[0], d_data.size());
}

//---------------------------------------------------------------------------//
// FREE FUNCTIONS
//---------------------------------------------------------------------------//

template<typename T>
void print_vec(const ThinVec<T>& v)
{
    cout << "Thinvec contents at " << &v << ": {";
    const char* sep = "";
    for (const double& val : v.data())
    {
        cout << sep << val;
        sep = ", ";
    }
    cout << "}\n";
}

void print_offbyone(const ThinVec<int>& INDICES)
{
    print_vec(INDICES);
}

//---------------------------------------------------------------------------//
// EXPLICIT INSTANTIATION
//---------------------------------------------------------------------------//

template class ThinVec<double>;
template class ThinVec<int>;

template void print_vec<double>(const ThinVec<double>&);
template void print_vec<int>(const ThinVec<int>&);

//---------------------------------------------------------------------------//
// end of thinvec/ThinVec.cc
//---------------------------------------------------------------------------//
