//---------------------------------*-SWIG-*----------------------------------//
/*!
 * \file   simple_class/simple.i
 * \author Seth R Johnson
 * \date   Thu Dec 01 15:07:35 2016
 * \note   Copyright (c) 2016 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//
%module(docstring="A simple example module") simple_class


%include "docstring.i"

%{
#include "SimpleClass.hh"
%}

#ifdef SWIGFORTRAN


%{
#include <iostream>
using std::cout;
using std::endl;
%}

%fortranbegin %{
! This code is injected immediately below the SWIG auto-generated comment
%}

%inline %{
void print_pointer(int msg, void* ptr)
{
    cout << "F " << (msg == 0 ? "Constructed" : "Releasing")
        << ' ' << ptr << endl;
}
%}


/*
 * Note: this used to be: \code
    write(0, "(a, z16)") "F Constructed ", self%swigptr
 * \endcode
 *
 * but printing pointers is not standards-compliant.
 */

%fortranappend SimpleClass::SimpleClass %{
    call print_pointer(0, self%swigptr)
%}
%fortranprepend SimpleClass::~SimpleClass %{
    call print_pointer(1, self%swigptr)
%}

#endif

// %ignore make_class;
// %ignore get_class;

%parameter approx_pi;


%feature("docstring") SimpleClass %{
Simple test class.

C++ includes: SimpleClass.hh
%};

%feature("docstring")  SimpleClass::double_it %{
void SimpleClass::double_it()

Multiply the value by 2.
%};

// %rename(SimpleClassDerp) SimpleClass;
%include "SimpleClass.hh"

// Overloaded instantiation
// %template(action) SimpleClass::action<double>;
// %template(action) SimpleClass::action<int>;

//---------------------------------------------------------------------------//
// end of simple_class/simple.i
//---------------------------------------------------------------------------//
