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
 *  Test for the l2_projection function.
 *  Bspline spaces case
 *
 *  author: pauletti
 *  date: 2013-10-31
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/base/function_lib.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/space_tools.h>


template<int dim , int range=1 ,int rank = 1>
void test_proj(const int p)
{
    using Space = BSplineSpace<dim,range,rank> ;
    using Func = typename functions::ConstantFunction<dim,range, rank>;

    const int num_knots = 4;
    auto knots = CartesianGrid<dim>::create(num_knots);
    auto space = Space::create(p, knots);

    const int n_qpoints = 4;
    QGauss<dim> quad(n_qpoints);

    typename Func::Value val;
    for (int i=0; i<range; ++i)
        val[i] = i+3;
    Func f(val);

#if defined(USE_TRILINOS)
    const auto la_pack = LAPack::trilinos;
#elif defined(USE_PETSC)
    const auto la_pack = LAPack::petsc;
#endif

    auto proj_values = space_tools::projection_l2<Space,la_pack>(f,space, quad);
    proj_values.print(out);
}



int main()
{
    out.depth_console(20);

    test_proj<1>(3);
    test_proj<2>(3);
    test_proj<3>(1);

    test_proj<2,3>(1);

    return 0;
}

