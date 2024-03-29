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
 *  Test for the push forward and reference element accessor retrieved from
 *  the physical space element accessor.
 *
 *  author: antolin
 *  date: 2014-04-03
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/geometry/push_forward.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/physical_space.h>
#include <igatools/basis_functions/physical_space_element_accessor.h>
#include <igatools/geometry/identity_mapping.h>

using namespace std ;
using namespace iga ;

template <int dim>
using RefSpace_t = BSplineSpace<dim>  ;

template <int dim>
using PushForward_t = PushForward<Transformation::h_grad,dim,0> ;

template <int dim>
using PhysicalSpace_t = PhysicalSpace< RefSpace_t<dim>, PushForward_t<dim> > ;

template <int dim>
void test_evaluate()
{
    out << "test_evaluate<" << dim << ">" << std::endl ;
    const int deg = 1;
    const int num_knot_spans = 1 ;
    auto grid = CartesianGrid<dim>::create(num_knot_spans+1);
    auto map = IdentityMapping<dim>::create(grid);

    auto push_forward = PushForward<Transformation::h_grad,dim>::create(map);
    auto ref_space = BSplineSpace<dim>::create(deg, grid);
    auto space = PhysicalSpace_t<dim>::create(ref_space, push_forward);

    auto elem = space->begin() ;
    const auto elem_end = space->end() ;
    const auto &elem_pf = elem->get_push_forward_accessor() ;
    const auto &elem_rs = elem->get_ref_space_accessor() ;

    const int n_qpoints = 1;
    QGauss<dim> quad(n_qpoints);
    ValueFlags flag = ValueFlags::map_gradient|ValueFlags::value;
    elem->init_cache(flag, quad);


    for (; elem != elem_end ; ++elem)
    {
        elem->fill_cache() ;
        elem->get_basis_values().print_info(out) ;
        out << endl;
        elem_pf.get_map_gradients().print_info(out) ;
        out << endl;
        elem_rs.get_basis_values().print_info(out) ;
        out << endl;
    }
    out << std::endl << std::endl ;
}

int main()
{
    test_evaluate<1>();
    test_evaluate<2>();
    test_evaluate<3>();
    return 0;
}
