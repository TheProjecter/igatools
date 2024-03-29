//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------
/*
 *  Test for the bspline element iterator
 *  Computes values and derivatives of the basis functions without the use of the cache.
 *  This test computes the same quantities of bspline_element_iterator_03.cpp
 *
 *  author: martinelli
 *  date: May 08, 2013
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/bspline_element_accessor.h>
#include <igatools/linear_algebra/distributed_vector.h>
#include <igatools/linear_algebra/dof_tools.h>

template< int dim_domain, int dim_range >
void do_test()
{
    auto grid = CartesianGrid<dim_domain>::create();

    const int degree = 1;
    const int rank =  1 ;
    typedef BSplineSpace< dim_domain, dim_range, rank > Space_t ;
    auto space = Space_t::create(degree, grid);

#if defined(USE_TRILINOS)
    const auto la_pack = LAPack::trilinos;
#elif defined(USE_PETSC)
    const auto la_pack = LAPack::petsc;
#endif
    Vector<la_pack> u(space->get_num_basis());
    {
        int id = 0 ;
        u(id++) = 0.0 ;
        u(id++) = 1.0 ;

        u(id++) = 0.0 ;
        u(id++) = 1.0 ;

        u(id++) = 0.0 ;
        u(id++) = 0.0 ;

        u(id++) = 1.0 ;
        u(id++) = 1.0 ;
    }

    QGauss< dim_domain > quad_scheme_1(2) ;
    const auto eval_points_1 = quad_scheme_1.get_points().get_flat_cartesian_product();

    auto element1 = space->begin();

    auto u_values = element1->evaluate_field_values_at_points(
                        u.get_local_coefs(element1->get_local_to_global()),
                        eval_points_1);
    u_values.print_info(out);
    auto values1    = element1->evaluate_basis_values_at_points(eval_points_1);
    values1.print_info(out);
    auto gradients1    = element1->evaluate_basis_gradients_at_points(eval_points_1);
    gradients1.print_info(out);

    QUniform< dim_domain > quad_scheme_2(3) ;
    const auto eval_points_2 = quad_scheme_2.get_points().get_flat_cartesian_product();

    auto values2    = element1->evaluate_basis_values_at_points(eval_points_2);
    values2.print_info(out);
    auto gradients2    = element1->evaluate_basis_gradients_at_points(eval_points_2);
    gradients2.print_info(out);

    u_values = element1->evaluate_field_values_at_points(
                   u.get_local_coefs(element1->get_local_to_global()),
                   eval_points_2);
    u_values.print_info(out);


}


int main(int argc, char *argv[])
{
    out.depth_console(10);

//   do_test<1,1>();
//    do_test<1,2>();
//    do_test<1,3>();
//
//    do_test<2,1>();
    do_test<2,2>();
//    do_test<2,3>();
//
//    do_test<3,1>();
    do_test<3,3>();

    return 0;
}
