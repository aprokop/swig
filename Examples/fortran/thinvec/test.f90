!-----------------------------------------------------------------------------!
! \file   thinvec/test.f90
! \author Seth R Johnson
! \date   Fri Dec 02 15:53:28 2016
! \brief  test module
! \note   Copyright (c) 2016 Oak Ridge National Laboratory, UT-Battelle, LLC.
!-----------------------------------------------------------------------------!

program main
    use ISO_FORTRAN_ENV
    use, intrinsic :: ISO_C_BINDING
    use thinvec, only : print_vec, print_offbyone, Vec => ThinVecInt
    implicit none
    integer :: i
    type(Vec) :: v
    integer(kind=c_int), dimension(3) :: dummy_data
    integer(kind=c_int), pointer, dimension(:) :: view

    ! This should be a null-op since the underlying pointer is initialized to
    ! null
    call v%release()

    write(0, *) "Constructing..."
    call v%create()
    write(0, *) "Sizing..."
    call v%resize(4)
    call print_vec(v)
    write(0, *) "Resizing with fill..."
    call v%resize_fill(10, -999)

    write(0, *) "Setting"
    do i = 0, 7
        call v%set(i, i * 2)
    end do
    call print_vec(v)
    write(0, *) "Printing with off-by-one"
    call print_offbyone(v)

    ! Pull data from the vector
    view => v%view()
    write(0, *) "View to ", size(view), "els:", view

    ! Assign a vector
    do i = 1, 3
        dummy_data(i) = i + 23
    end do
    write(0, *) "Assigning vector", dummy_data
    call v%assign_from(dummy_data)
    call print_vec(v)

    write(0, *) "Destroying..."
    call v%release()
end program

!-----------------------------------------------------------------------------!
! end of thinvec/test.f90
!-----------------------------------------------------------------------------!
