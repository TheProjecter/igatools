#-+--------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
#
# This file is part of the igatools library.
#
# The igatools library is free software: you can use it, redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-+--------------------------------------------------------------------

from init_instantiation_data import *

include_files = ['geometry/cartesian_grid.h',
                 'geometry/cartesian_grid_element_accessor.h',
                 'basis_functions/bspline_space.h',
                 '../../source/geometry/grid_forward_iterator.cpp']
data = Instantiation(include_files)
(f, inst) = (data.file_output, data.inst)

accessors = ['NURBSElementAccessor<%d, %d, %d>' %(x.dim, x.range, x.rank)  
          for x in inst.really_all_ref_sp_dims ]
for accessor in accessors:
   f.write('template class %s ;\n' %accessor)
   f.write('template class GridForwardIterator<%s> ;\n' %accessor)
   function = ('template  ValueTable< Conditional< deriv_order==0,'+
               accessor + '::Value,' +
               accessor + '::Derivative<deriv_order> > > ' + 
               accessor + 
               '::evaluate_basis_derivatives_at_points<deriv_order>' +
               '(const ValueVector<Point>&) const; \n')
   fun_list = [function.replace('deriv_order', str(d)) for d in inst.deriv_order]
   for s in fun_list:
      f.write(s)
