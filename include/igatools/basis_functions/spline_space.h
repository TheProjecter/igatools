//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
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

#ifndef SPACE_SPEC_H_
#define SPACE_SPEC_H_

#include <igatools/base/config.h>
#include <igatools/base/array_utils.h>
#include <igatools/base/function.h>
#include <igatools/utils/cartesian_product_array.h>
#include <igatools/utils/static_multi_array.h>
#include <igatools/utils/dynamic_multi_array.h>
#include <igatools/basis_functions/function_space.h>
#include <igatools/geometry/cartesian_grid.h>


IGA_NAMESPACE_OPEN

/**
 * @brief Tensor product spline space
 *
 * A one dimensional polynomial spline space is determined by:
 * - a domain, the interval [a,b]
 * - the polynomial order
 * - a partition of [a,b], the knots
 * - the interior knots smoothness (given by the interior multiplicity)
 *
 * This class provides the realization of a scalar, vector or k-tensor
 * value tensor product spline space.
 *
 * @note This is independent of the basis functions one may wish to use
 * for the given space.
 *
 * @author pauletti, 2014
 *
 */
template<int dim, int range = 1, int rank = 1>
class SplineSpace :
    public FunctionSpaceOnGrid<CartesianGrid<dim>>
{

private:
    using GridSpace = FunctionSpaceOnGrid<CartesianGrid<dim>>;
    using typename GridSpace::GridType;

public:
    static const std::array<int, dim> dims;

    using FaceSpace = Conditional<(dim>0),
          SplineSpace<dim-1, range, rank>,
          SplineSpace<    0, range, rank> >;
public:
    using Func = Function<dim, range, rank>;

public:
    template <int order>
    using Derivative = typename Func::template Derivative<order>;
    using Point = typename Func::Point;
    using Value = typename Func::Value;
    using Div   = typename Func::Div;

public:
    template<class> class ComponentContainer;
    static constexpr int n_components = ComponentContainer<int>::n_entries;
    static const std::array<int, n_components> components;

public:
    using Knots = CartesianProductArray<Real, dim>;
    using BoundaryKnots = std::array<CartesianProductArray<Real,2>, dim>;
    using Degrees  = TensorIndex<dim>;
    using Multiplicity = CartesianProductArray<Size, dim>;

    using DegreeTable = ComponentContainer<Degrees>;
    using MultiplicityTable = ComponentContainer<Multiplicity>;
    using BoundaryKnotsTable = ComponentContainer<BoundaryKnots>;
    using KnotsTable = ComponentContainer<Knots>;
    using PeriodicTable = ComponentContainer<std::array<bool, dim> >;

    using IndexSpaceTable = ComponentContainer<DynamicMultiArray<Index,dim>>;
    using IndexSpaceMarkTable = Multiplicity;

    class SpaceDimensionTable : public ComponentContainer<TensorSize<dim> >
    {
    public:
        ComponentContainer<Size> comp_dimension;
        Size total_dimension;
    };

    // For the boundary kntos types
    // interpolatory (open knot)
    enum class EndBehaviour
    {
        interpolatory
    };

    // For the interior multiplicities
    // maximum regularity
    // minimul regularity discontinous
    enum class InteriorReg
    {
        maximum, minimun
    };

public:
    /**
     * Most general constructor
     */
    explicit SplineSpace(const DegreeTable &deg,
                         std::shared_ptr<GridType> knots,
                         std::shared_ptr<const MultiplicityTable> interior_mult,
                         const PeriodicTable periodic = PeriodicTable(filled_array<bool,dim>(false)));

    explicit SplineSpace(const DegreeTable &deg,
                         std::shared_ptr<GridType> knots,
                         const InteriorReg interior_mult,
                         const PeriodicTable periodic = PeriodicTable(filled_array<bool,dim>(false)))
        :SplineSpace(deg, knots, fill_max_regularity(deg, knots), periodic)
    {}

    const DegreeTable &get_degree() const
    {
        return deg_;
    }



    /** @name Getting information about the space */
    ///@{
    /**
     * Total number of basis functions. This is the dimensionality
     * of the space.
     */
    Size get_num_basis() const
    {
        return space_dim_.total_dimension;
    }

    /**
     * Total number of basis functions
     * for the comp space component.
     */
    Size get_num_basis(const int comp) const
    {
        return space_dim_.comp_dimension(comp);
    }

    /**
     *  Total number of basis functions for the comp space component
     *  and the dir direction.
     */
    Size get_num_basis(const int comp, const int dir) const
    {
        return  space_dim_(comp)[dir];
    }

    /**
     * Component-direction indexed table with the number of basis functions
     * in each direction and component
     */
    const SpaceDimensionTable &get_num_basis_table() const
    {
        return space_dim_;
    }

    const SpaceDimensionTable &get_num_basis_per_element_table() const
    {
        return elem_n_basis_;
    }
    /**
     * Returns the number of dofs per element.
     */
    Size get_num_basis_per_element() const
    {
        return elem_n_basis_.total_dimension;
    }

    /**
     *  Return the number of dofs per element for the i-th space component.
     */
    Size get_num_basis_per_element(int i) const
    {
        return elem_n_basis_.comp_dimension(i);
    }

    ///@}

    /**
     * Returns the multiplicity of the face space face_id
     */
    std::shared_ptr<typename FaceSpace::MultiplicityTable>
    get_face_mult(const Index face_id) const;

    /**
     * Returns the multiplicity of the face space face_id
     */
    typename FaceSpace::DegreeTable
    get_face_degree(const Index face_id) const;


    KnotsTable compute_knots_with_repetition(const BoundaryKnotsTable &boundary_knots);

    KnotsTable compute_knots_with_repetition(const EndBehaviour type)
    {
        return compute_knots_with_repetition(interpolatory_end_knots());
    }

    /**
     * For each element and for each component there is an initial
     * tensor index in the Index space from where all non-zero basis
     * function can be determined.
     */
    MultiplicityTable accumulated_interior_multiplicities() const;



    void print_info(LogStream &out) const;

private:
    /**
     * Fill the multiplicy for the maximum possible regularity
     *  of the given number of knots
     */
    std::shared_ptr<MultiplicityTable> fill_max_regularity(const DegreeTable &deg, std::shared_ptr<const GridType> grid);

    BoundaryKnotsTable interpolatory_end_knots();



private:
    std::shared_ptr<const MultiplicityTable> interior_mult_;

    DegreeTable deg_;

    /** Table with the dimensionality of the space in each component and direction */
    SpaceDimensionTable space_dim_;

    /** Table with the number of element non zero basis in each component and direction */
    SpaceDimensionTable elem_n_basis_;

    PeriodicTable periodic_;

public:
    /**
     *  Class to manage the component quantities with the knowledge of
     * uniform range spaces
     */
    template<class T>
    class ComponentContainer : public StaticMultiArray<T,range,rank>
    {
        using base_t = StaticMultiArray<T,range,rank>;
    public:
        // using base_t::Entry;
        /** Type of the const iterator. */
        using const_iterator =  MultiArrayIterator<const ComponentContainer<T>>;
    public:
        using base_t::n_entries;

        using  ComponentMap = std::array <Index, n_entries>;

        ComponentContainer(const ComponentMap &comp_map =
                               sequence<n_entries>());

        /**
         * Construct a homogenous range table with val value
         */
        ComponentContainer(const T &val);

        ComponentContainer(std::initializer_list<T> list);

        const_iterator
        cbegin() const
        {
            return const_iterator(*this,0);
        }

        const_iterator
        cend() const
        {
            return const_iterator(*this,IteratorState::pass_the_end);
        }


        const_iterator
        begin() const
        {
            return cbegin();
        }

        const_iterator
        end() const
        {
            return cend();
        }

        /**
         *  Flat index access operator (non-const version).
         */
        T &operator()(const Index i);

        /**
         *  Flat index access operator (const version).
         */
        const T &operator()(const Index i) const;

        const Index active(const Index i) const
        {
            return comp_map_[i];
        }

        const std::vector<Index> &get_active_components() const
        {
            return active_components_;
        }

        const std::vector<Index> &get_inactive_components() const
        {
            return inactive_components_;
        }

        void
        print_info(LogStream &out) const
        {
            for (int i=0; i<n_entries; ++i)
                out << (*this)(i) << " ";
        }

        const std::array <Index, n_entries> &get_comp_map() const
        {
            return comp_map_;
        }

    private:
        /** For each component return de index of the active component */
        std::array <Index, n_entries> comp_map_;

        /** list of the active components */
        std::vector<Index> active_components_;

        /** list of the inactive components */
        std::vector<Index> inactive_components_;


    };

};


template <class T, int dim>
inline
std::vector<T>
unique_container(std::array <T, dim> a)
{
    auto it = std::unique(a.begin(), a.end());
    return std::vector<T>(a.begin(), it);
}



template<int dim, int range, int rank>
template<class T>
SplineSpace<dim, range, rank>::ComponentContainer<T>::
ComponentContainer(const ComponentMap &comp_map)
    :
    base_t(),
    comp_map_(comp_map),
    active_components_(unique_container<Index, n_entries>(comp_map)),
    inactive_components_(n_entries)
{
    auto all = sequence<n_entries>();
    auto it=std::set_difference(all.begin(), all.end(), active_components_.begin(),
                                active_components_.end(), inactive_components_.begin());

    inactive_components_.resize(it-inactive_components_.begin());
}



template<int dim, int range, int rank>
template<class T>
SplineSpace<dim, range, rank>::ComponentContainer<T>::
ComponentContainer(std::initializer_list<T> list)
    :
    base_t(list),
    comp_map_(sequence<n_entries>()),
    active_components_(unique_container<Index, n_entries>(comp_map_))
{}



template<int dim, int range, int rank>
template<class T>
SplineSpace<dim, range, rank>::ComponentContainer<T>::
ComponentContainer(const T &val)
    :
    comp_map_(filled_array<Index, n_entries>(0)),
    active_components_(1,0),
    inactive_components_(n_entries-1)
{
    for (int i=1; i<n_entries; ++i)
        inactive_components_[i-1] = i;

    base_t::operator()(0) = val;
}



template<int dim, int range, int rank>
template<class T>
T &
SplineSpace<dim, range, rank>::ComponentContainer<T>::
operator()(const Index i)
{
    return base_t::operator()(comp_map_[i]);
}


template<int dim, int range, int rank>
template<class T>
const T &
SplineSpace<dim, range, rank>::ComponentContainer<T>::
operator()(const Index i) const
{
    return base_t::operator()(comp_map_[i]);
}

IGA_NAMESPACE_CLOSE

#endif