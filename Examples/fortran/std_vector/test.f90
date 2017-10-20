!-----------------------------------------------------------------------------!
! \file   std_vector/test.f90
! \author Seth R Johnson
! \date   Mon Dec 05 09:05:31 2016
! \brief  test module
! \note   Copyright (c) 2016 Oak Ridge National Laboratory, UT-Battelle, LLC.
!-----------------------------------------------------------------------------!

program main
    use ISO_FORTRAN_ENV
    use, intrinsic :: ISO_C_BINDING
    use stdvec, only : make_const_view => make_const_viewdbl, &
        print_view => print_viewdbl, const_VecViewDbl, VecDbl, &
        print_viewptr
    implicit none
    type(VecDbl) :: v
    type(const_VecViewDbl) :: cview
    integer :: i
    real(C_DOUBLE), pointer :: vptr(:)

    ! This should be a null-op since the underlying pointer is initialized to
    ! null
    call v%release()
    
    write(0, *) "Constructing..."
    call v%create()
    write(0, *) "Sizing..."
    call v%resize(4)
    write(0, *) "Resizing with fill..."
    call v%resize(10, 1.5d0)

    write(0, *) "Setting"
    do i = 0, 7
        call v%set(i, real(i + 1) * 123.0d0)
    end do

    ! Get view
    write(0, *) "Getting and printing const view"
    cview = make_const_view(v)
    call print_view(cview)

    vptr => cview%view()
    write(0, *) "pointer:", vptr

    write(0, *) "Printing from array pointer"
    call print_viewptr(vptr)

    write(0, *) "Destroying..."
    call v%release()
end program

!-----------------------------------------------------------------------------!
! end of std_vector/test.f90
!-----------------------------------------------------------------------------!
