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

# QA (pauletti, Mar 19, 2014): NOT PASSED
# todo use prrper ref to phys dims in <0,0>
from init_instantiation_data import *
include_files = ['geometry/mapping.h',
                 '../../source/geometry/grid_forward_iterator.cpp',
                 'basis_functions/bspline_element_accessor.h',
                 'basis_functions/nurbs_element_accessor.h']
data = Instantiation(include_files)
(f, inst) = (data.file_output, data.inst)

mappings = ['MappingElementAccessor<%d, %d>' %(x.dim, x.codim) for x in inst.all_mapping_dims]
mappings = mappings + ['MappingElementAccessor<0, 0>'] #todo use porper ref to phys dims
for row in mappings:
    f.write('template class %s; \n' % (row))
    f.write('template class GridForwardIterator<%s>; \n'% (row))
