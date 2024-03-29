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

// [old includes]
#include <igatools/base/function_lib.h>
#include <igatools/base/quadrature_lib.h>
#include <igatools/geometry/mapping_lib.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/physical_space.h>
#include <igatools/basis_functions/physical_space_element_accessor.h>
#include <igatools/basis_functions/space_tools.h>
#include <igatools/linear_algebra/dense_matrix.h>
#include <igatools/linear_algebra/dense_vector.h>
#include <igatools/linear_algebra/distributed_matrix.h>
#include <igatools/linear_algebra/distributed_vector.h>
#include <igatools/linear_algebra/linear_solver.h>
#include <igatools/linear_algebra/sparsity_pattern.h>
#include <igatools/linear_algebra/dof_tools.h>
#include <igatools/io/writer.h>
// [old includes]

// [unqualified names]
using namespace iga;
using namespace std;
using functions::ConstantFunction;
using space_tools::project_boundary_values;
using dof_tools::apply_boundary_values;
using numbers::PI;
// [unqualified names]

// [Problem class]
template<int dim>
class PoissonProblem
{
public:
    PoissonProblem(const int deg, const TensorSize<dim> &n_knots);
    void run();

private:
    void assemble();
    void solve();
    void output();
    // [Problem class]

    // [type aliases]
private:
    using RefSpace  = BSplineSpace<dim>;
    using PushFw    = PushForward<Transformation::h_grad, dim>;
    using Space     = PhysicalSpace<RefSpace, PushFw>;
    using Value = typename Function<dim>::Value;
    // [type aliases]

    shared_ptr<Mapping<dim>> map;
    shared_ptr<Space>        space;

    const Quadrature<dim>   elem_quad;
    const Quadrature<dim-1> face_quad;

    const boundary_id dir_id = 0;

    std::shared_ptr<Matrix<LAPack::trilinos>> matrix;
    std::shared_ptr<Vector<LAPack::trilinos>> rhs;
    std::shared_ptr<Vector<LAPack::trilinos>> solution;
};



template<int dim>
PoissonProblem<dim>::
PoissonProblem(const int deg, const TensorSize<dim> &n_knots)
    :
    elem_quad(QGauss<dim>(deg+1)),
    face_quad(QGauss<dim-1>(deg+1))
{
    BBox<dim> box;
    box[0] = {{0.5,1}};
    for (int i=1; i<dim; ++i)
        box[i] = {{PI/4,PI/2}};

    auto grid = CartesianGrid<dim>::create(box, n_knots);
    auto ref_space = RefSpace::create(deg, grid);
    map       = BallMapping<dim>::create(grid);
    space     = Space::create(ref_space, PushFw::create(map));

    const auto n_basis = space->get_num_basis();
    matrix   = Matrix<LAPack::trilinos>::create(*space->get_space_manager());
    rhs      = Vector<LAPack::trilinos>::create(n_basis);
    solution = Vector<LAPack::trilinos>::create(n_basis);
}



template<int dim>
void PoissonProblem<dim>::assemble()
{
    const int n_qp = elem_quad.get_num_points();
    ConstantFunction<dim> f({0.5});
    ValueVector<Value> f_values(n_qp);

    auto elem = space->begin();
    const auto elem_end = space->end();
    ValueFlags fill_flags = ValueFlags::value | ValueFlags::gradient |
                            ValueFlags::w_measure | ValueFlags::point;
    elem->init_cache(fill_flags, elem_quad);

    for (; elem != elem_end; ++elem)
    {
        const int n_basis = elem->get_num_basis();
        DenseMatrix loc_mat(n_basis, n_basis);
        loc_mat = 0.0;

        DenseVector loc_rhs(n_basis);
        loc_rhs = 0.0;

        elem->fill_cache();

        auto points  = elem->get_points();
        auto phi     = elem->get_basis_values();
        auto grd_phi = elem->get_basis_gradients();
        auto w_meas  = elem->get_w_measures();

        f.evaluate(points, f_values);

        for (int i = 0; i < n_basis; ++i)
        {
            auto grd_phi_i = grd_phi.get_function_view(i);
            for (int j = 0; j < n_basis; ++j)
            {
                auto grd_phi_j = grd_phi.get_function_view(j);
                for (int qp = 0; qp < n_qp; ++qp)
                    loc_mat(i,j) +=
                        scalar_product(grd_phi_i[qp], grd_phi_j[qp])
                        * w_meas[qp];
            }

            auto phi_i = phi.get_function_view(i);
            for (int qp = 0; qp < n_qp; ++qp)
                loc_rhs(i) += scalar_product(phi_i[qp], f_values[qp])
                              * w_meas[qp];
        }

        auto loc_dofs = elem->get_local_to_global();
        matrix->add_block(loc_dofs, loc_dofs, loc_mat);
        rhs->add_block(loc_dofs, loc_rhs);
    }

    matrix->fill_complete();

    // [dirichlet constraint]
    ConstantFunction<dim> g({0.0});
    std::map<Index, Real> values;
    project_boundary_values<Space,LAPack::trilinos>(g,space,face_quad,dir_id,values);
    apply_boundary_values(values, *matrix, *rhs, *solution);
    // [dirichlet constraint]
}



template<int dim>
void PoissonProblem<dim>::solve()
{
    using LinSolver = LinearSolver<LAPack::trilinos>;
    LinSolver solver(LinSolver::SolverType::CG);
    solver.solve(*matrix, *rhs, *solution);
}



template<int dim>
void PoissonProblem<dim>::output()
{
    const int n_plot_points = 2;
    Writer<dim> writer(map, n_plot_points);

    writer.add_field(space, *solution, "solution");
    string filename = "poisson_problem-" + to_string(dim) + "d" ;
    writer.save(filename);
}



template<int dim>
void PoissonProblem<dim>::run()
{
    assemble();
    solve();
    output();
}



int main()
{
    const int n_knots = 10;
    const int deg     = 1;

    PoissonProblem<1> poisson_1d(deg, {n_knots});
    poisson_1d.run();

    PoissonProblem<2> poisson_2d(deg, {n_knots, n_knots});
    poisson_2d.run();

    PoissonProblem<3> poisson_3d(deg, {n_knots, n_knots, n_knots});
    poisson_3d.run();

    return  0;
}
