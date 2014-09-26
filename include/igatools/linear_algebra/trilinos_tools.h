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

#ifndef TRILINOS_TOOLS_H_
#define TRILINOS_TOOLS_H_

#include <igatools/base/config.h>

#include <igatools/basis_functions/space_manager.h>

#ifdef USE_TRILINOS
#include <Tpetra_CrsMatrix.hpp>

IGA_NAMESPACE_OPEN





/** Type alias for the local ordinal types (i.e. the types for the local indices). */
using LO = Index;

/** Type alias for the global ordinal types (i.e. the types for the global indices). */
using GO = Index;

/** Type alias for the communicator. */
using Comm = Teuchos::Comm<int>;
using CommPtr = Teuchos::RCP<const Comm>;

/** Type alias for the dofs map across the processors */
using DofsMap = Tpetra::Map<LO,GO>;
using DofsMapPtr = Teuchos::RCP<const DofsMap>;

/** Type alias for the connectivty graph */
using Graph = Tpetra::CrsGraph<LO,GO>;
using GraphPtr = Teuchos::RCP<Graph>;

/** Type alias for the Trilinos matrix. */
using MatrixImpl = Tpetra::CrsMatrix<Real,LO,GO>;
using MatrixImplPtr = Teuchos::RCP<MatrixImpl>;


namespace trilinos_tools
{
DofsMapPtr build_row_map(const SpaceManager &space_manager, const CommPtr comm);

DofsMapPtr build_col_map(const SpaceManager &space_manager, const CommPtr comm);

GraphPtr build_graph(const SpaceManager &space_manager,const DofsMapPtr row_map,const DofsMapPtr col_map);

MatrixImplPtr build_matrix(GraphPtr graph);
};


IGA_NAMESPACE_CLOSE

#endif // #ifdef USE_TRILINOS

#endif // #ifndef TRILINOS_TOOLS_H_