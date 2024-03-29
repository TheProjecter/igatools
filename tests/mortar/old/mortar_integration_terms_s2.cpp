
#include <igatools/base/function_lib.h>
#include <igatools/geometry/mapping.h>
#include <igatools/geometry/mapping_lib.h>
#include <igatools/geometry/identity_mapping.h>
#include <igatools/geometry/ig_mapping.h>
#include <igatools/geometry/cartesian_grid.h>
#include <igatools/geometry/cartesian_grid_element_accessor.h>
#include <igatools/geometry/topology.h>
//#include <igatools/io/reader.h>
#include <igatools/basis_functions/space_tools.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/physical_space.h>
#include <igatools/basis_functions/physical_space_element_accessor.h>
#include <igatools/basis_functions/multi_patch_space.h>
#include <igatools/io/writer.h>
#include <iostream> 
#include <iterator>

#include <igatools/base/quadrature_lib.h>
#include <igatools/linear_algebra/dense_matrix.h>
#include <igatools/linear_algebra/dense_vector.h>
#include <igatools/linear_algebra/distributed_matrix.h>
#include <igatools/linear_algebra/distributed_vector.h>
#include <igatools/linear_algebra/linear_solver.h>
#include <igatools/linear_algebra/dof_tools.h>

#include <math.h>

using namespace iga;
using namespace std;
using functions::ConstantFunction;
using space_tools::project_boundary_values;
using dof_tools::apply_boundary_values;
using dof_tools::get_sparsity_pattern;




template <int dim>
shared_ptr<Mapping<dim,0>> create_map(const vector<Real> &control_pts)
{
	auto knots = CartesianGrid<dim>::create(2);
	
	using RefSpaceMap = BSplineSpace<dim,dim,1>;
	
	const int p = 1;
	auto ref_space_map = RefSpaceMap::create(p,knots);
	
	Assert(control_pts.size() == ref_space_map->get_num_basis(),
		   ExcDimensionMismatch(control_pts.size(),ref_space_map->get_num_basis()));
	
	return IgMapping<RefSpaceMap>::create(ref_space_map, control_pts);
}




template <int dim>
shared_ptr<Mapping<dim,0>> create_geo_0(){
	const int n_basis = dim * std::pow(2,dim);
	vector<Real> control_pts(n_basis);
	
	if (dim == 1)
	{
		int id = 0 ;
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 1.0 ;
	}
	else if (dim == 2) // x coordinates of all the points, then y
	{
		int id = 0 ;
		control_pts[id++] = 0.0 ; 
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 0.0 ;
		
		control_pts[id++] = 1.0 ;
		control_pts[id++] = 1.0 ;
	}
	else if (dim == 3)
	{
		int id = 0 ;
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 0.0 ;
		
		control_pts[id++] = 1.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 0.0 ;
		
		control_pts[id++] = 1.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 0.0 ;
		
		control_pts[id++] = 0.0 ;
		control_pts[id++] = 0.0 ;
		
		control_pts[id++] = 1.0 ;
		control_pts[id++] = 1.0 ;
		
		control_pts[id++] = 1.0 ;
		control_pts[id++] = 1.0 ;
		
	}
	return create_map<dim>(control_pts);
}



template <int dim>
shared_ptr<Mapping<dim,0>> create_geo_1(){
	const int n_basis = dim * std::pow(2,dim);
	vector<Real> control_pts(n_basis);
	
	if (dim == 1)
	{
		control_pts[0] = 1.0 ;
		control_pts[1] = 2.0 ;
	}
	else if (dim == 2)
	{
		control_pts[0] = 1.0 ;
		control_pts[1] = 2.0 ;
		
		control_pts[2] = 1.0 ;
		control_pts[3] = 2.0 ;
		
		control_pts[4] = 0.0 ;
		control_pts[5] = 0.0 ;
		
		control_pts[6] = 1.0 ;
		control_pts[7] = 1.0 ;
	}
	else if (dim == 3)
	{
		control_pts[0] = 1.0 ;
		control_pts[1] = 2.0 ;
		control_pts[2] = 1.0 ;
		
		control_pts[3] = 2.0 ;
		control_pts[4] = 1.0 ;
		control_pts[5] = 2.0 ;
		
		control_pts[6] = 1.0 ;
		control_pts[7] = 2.0 ;
		control_pts[8] = 0.0 ;
		
		control_pts[9] = 0.0 ;
		control_pts[10] = 1.0 ;
		control_pts[11] = 1.0 ;
		
		control_pts[12] = 0.0 ;
		control_pts[13] = 0.0 ;
		control_pts[14] = 1.0 ;
		
		control_pts[15] = 1.0 ;
		control_pts[16] = 0.0 ;
		control_pts[17] = 0.0 ;
		
		control_pts[18] = 0.0 ;
		control_pts[19] = 0.0 ;
		control_pts[20] = 1.0 ;
		
		control_pts[21] = 1.0 ;
		control_pts[22] = 1.0 ;
		control_pts[23] = 1.0 ;
	}
	
	return create_map<dim>(control_pts);
}



//real multiply_vect(real i, real val) {i=i*val; return i}



int main()
{
	
    
	LogStream out;
	const int dim(2);
	const int dim_field(1);
	vector<int> degrees(2,1);
	const int deg(1);
	
	
	// Mapping $\R^2 \rightarrow \R^2$, scalar field
	vector<shared_ptr<Mapping<dim,0>>> maps_d21;
	maps_d21.push_back(create_geo_0<dim>());
	maps_d21.push_back(create_geo_1<dim>());
	
	using RefSpaceField         = BSplineSpace<dim,dim_field,1>;
	using RefSpaceField_ptr     = shared_ptr<RefSpaceField>;
	using PushFw                = PushForward<Transformation::h_grad, dim,0>;
	using PhySpace              = PhysicalSpace<RefSpaceField, PushFw>;
	using PhySpace_ptr          = shared_ptr<PhySpace>;	   
	
	using Func = Function<dim, dim>;
	using Gradient = typename Func::Gradient;
	
	vector<RefSpaceField_ptr>	ref_spaces_field;
	vector<PhySpace_ptr>	    spaces;
	
	MultiPatchSpace<PhySpace> domain_multip;
	domain_multip.arrangement_open();
	const boundary_id mortar_id = 7;
	vector<set<boundary_id>> mortar_faces;
	mortar_faces.push_back({1});
	mortar_faces.push_back({0});
	
	
	for(uint i=0; i!=maps_d21.size(); ++i){
		ref_spaces_field.push_back(RefSpaceField::create(degrees[i], maps_d21[i]->get_grid()));
		spaces.push_back(PhySpace::create(ref_spaces_field[i], PushFw::create(maps_d21[i]),i));
		//		spaces[i]->print_info(out);
		if (i==1){
			spaces[i]->refine_h(2); 
		}// Problem here with a value different than 2
		spaces[i]->print_info(out);
		////This should be done before:
		auto tmp_mortar_faces=mortar_faces[i];
		auto crr_mortar=tmp_mortar_faces.begin(), end_mortar=tmp_mortar_faces.end();
		for (; crr_mortar!=end_mortar; ++crr_mortar)
		{
			spaces[i]->get_grid()->set_boundary_id(*crr_mortar,mortar_id);
		}
		/////
		
		domain_multip.add_patch(spaces[i]);
	}
	
	domain_multip.arrangement_close();
	
	
	// multiplier space 
	const int degree_multiplier=1;
	auto ref_space_multiplier = RefSpaceField::create(degree_multiplier,spaces[0]->get_grid());
	auto multiplier_space=PhySpace::create(ref_space_multiplier, PushFw::create(maps_d21[0]),1);
	//multiplier_space->print_info(out);
	
	const int n_basis0 = spaces[0]->get_num_basis_per_element();
	const int n_basis1 = spaces[1]->get_num_basis_per_element();
	const int n_basism = multiplier_space->get_num_basis_per_element();
	
	const int nt_basis0 = spaces[0]->get_num_basis();
	const int nt_basis1 = spaces[1]->get_num_basis();
	const int nt_basism = multiplier_space->get_num_basis();
	
	//out<<"n_basis0"<<n_basis0<<"n_basis1"<<n_basis1<<"n_basism"<<n_basism;
	//out<<"nt_basis0"<<nt_basis0<<"nt_basis1"<<nt_basis1<<"nt_basism"<<nt_basism;
	
	vector< Index > joint_to_slave;
	vector< Index > joint_to_master;
	auto slave_grid=spaces[0]->get_grid();
	auto master_grid=spaces[1]->get_grid();
	
	vector<shared_ptr<CartesianGrid<dim>>> joint_grid;
	joint_grid.push_back(make_shared<CartesianGrid<dim>>(build_cartesian_grid_union(*slave_grid, *master_grid, joint_to_slave, joint_to_master))); 	//pointeur for the iterator
    joint_grid.push_back(make_shared<CartesianGrid<dim>>(build_cartesian_grid_union(*slave_grid, *master_grid, joint_to_slave, joint_to_master)));
	//map0->print_info(out);
	//map1->print_info(out);
	joint_grid[0]->print_info(out);
	joint_grid[1]->print_info(out);
	//auto bd_id0_0=grid_Js->get_boundary_id(0);
	auto tmp0_bd_id0_0=joint_grid[0]->get_boundary_id(0);
	auto tmp0_bd_id0_1=joint_grid[0]->get_boundary_id(1);
	auto tmp0_bd_id0_2=joint_grid[0]->get_boundary_id(2);
	auto tmp0_bd_id0_3=joint_grid[0]->get_boundary_id(3);
	out<< to_string(tmp0_bd_id0_0)<< to_string(tmp0_bd_id0_1)<< to_string(tmp0_bd_id0_2)<< to_string(tmp0_bd_id0_3)<<endl;
	
	
	for(uint i=0; i!=joint_grid.size(); ++i){
		auto tmp_mortar_faces=mortar_faces[i];
		auto crr_mortar=tmp_mortar_faces.begin(), end_mortar=tmp_mortar_faces.end();
		for (; crr_mortar!=end_mortar; ++crr_mortar){
			joint_grid[i]->set_boundary_id(*crr_mortar,mortar_id);
		}
	}
	
	
	
	auto tmp_bd_id0_0=joint_grid[0]->get_boundary_id(0);
	auto tmp_bd_id0_1=joint_grid[0]->get_boundary_id(1);
	auto tmp_bd_id0_2=joint_grid[0]->get_boundary_id(2);
	auto tmp_bd_id0_3=joint_grid[0]->get_boundary_id(3);
	out<< to_string(tmp_bd_id0_0)<< to_string(tmp_bd_id0_1)<< to_string(tmp_bd_id0_2)<< to_string(tmp_bd_id0_3)<<endl;
	
	
	const Quadrature<dim>   joint_elem_quad(QGauss<dim>(deg+1));
    const Quadrature<dim-1> joint_face_quad(QGauss<dim-1>(deg+1));
	const int n_qp = joint_face_quad.get_num_points();
	
	auto  slave_joint_face_quad_gbl   = extend_face_quad(joint_face_quad,1);
	auto  master_joint_face_quad_gbl  = extend_face_quad(joint_face_quad,0);
	auto slave_pts_unit_domain        = slave_joint_face_quad_gbl.get_points().get_flat_cartesian_product();
	auto master_pts_unit_domain       = master_joint_face_quad_gbl.get_points().get_flat_cartesian_product();
	auto w_unit_domain    = slave_joint_face_quad_gbl.get_weights().get_flat_tensor_product();
	out<<to_string(w_unit_domain[0])<<endl;
	
	//int num_el;
	//num_el+=num_el;
	//auto elem_Jm=next(*joint_grid[1]->begin(), num_el);
	
	vector<Index> elem_slave_id;
	vector<Index> elem_master_id;
	auto elem_Jm=*joint_grid[1]->begin();
	for(auto elem_Js: *joint_grid[0]){
		if (elem_Js.is_boundary()){
			for (Index face_id = 0; face_id < UnitElement<dim>::faces_per_element; ++face_id){
				if (elem_Js.is_boundary(face_id)){
					auto 	s_grid_tmp=elem_Js.get_grid();
					//out<<to_string(s_grid_tmp->get_boundary_id(face_id))<<endl;					
					if (s_grid_tmp->get_boundary_id(face_id)==mortar_id){
						auto elem_Js_id     = elem_Js.get_flat_index();
						elem_slave_id.push_back(joint_to_slave[elem_Js_id]);
 						out<<"SLAVE ID"<<elem_Js_id<<joint_to_slave[elem_Js_id]<<endl;
					}
				}
				if (elem_Jm.is_boundary(face_id)){
					auto 	m_grid_tmp=elem_Jm.get_grid();
					//out<<to_string(m_grid_tmp->get_boundary_id(face_id))<<endl;	
					if (m_grid_tmp->get_boundary_id(face_id)==mortar_id){
						auto elem_Jm_id     = elem_Js.get_flat_index();
						elem_master_id.push_back(joint_to_master[elem_Jm_id]); 	
						out<<"MASTER ID"<<elem_Jm_id<<joint_to_master[elem_Jm_id]<<endl;
					}
				}
			}
		}
		++elem_Jm;
	}
	
	using term_coef=pair<Index, Real>;
	using LinearConstraint=pair<vector<term_coef>,Real>;
	vector<LinearConstraint> LCs(nt_basism);
	DenseMatrix loc_mat0(nt_basism, nt_basis0);
 	DenseMatrix loc_mat1(nt_basism, nt_basis1);
	loc_mat0.clear();
	loc_mat1.clear();
	
	int elem_nb(0);
	for(auto elem_Js: *joint_grid[0]){
		if (elem_Js.is_boundary()){
			for (Index face_id = 0; face_id < UnitElement<dim>::faces_per_element; ++face_id){
				if (elem_Js.is_boundary(face_id)){
					auto loc_to_globm=elem_Js->get_loc_to_global();
					auto loc_to_glob0=elem_Js->get_loc_to_global();
					auto grid_tmp=elem_Js.get_grid();					
					if (grid_tmp->get_boundary_id(face_id)==mortar_id){
						
						auto quad_ref_domain = elem_Js.transform_points_unit_to_reference(slave_pts_unit_domain);
						const auto w_meas=static_cast<CartesianGridElement<dim>&>(elem_Js).get_measure(FaceTopology<dim>(1));
						out<<w_meas<<endl;
						//
						auto elem_slave     = spaces[0]->get_element(elem_slave_id[elem_nb]);
						auto elem_multiplier= multiplier_space->get_element(elem_slave_id[elem_nb]);
						auto elem_master    = spaces[1]->get_element(elem_master_id[elem_nb]);
						vector<Gradient> grad_0;
						maps_d21[0]->evaluate_gradients_at_points(quad_ref_domain,grad_0);//slave_pts_unit_domain, grad_0);
						auto det=determinant<dim,dim>(grad_0[0]);
						out<<det<<"ICI"<<endl;
						//					
						//					 
						auto basis_slave=elem_slave.evaluate_basis_values_at_points(slave_pts_unit_domain);
						auto basis_multiplier=elem_multiplier.evaluate_basis_values_at_points(slave_pts_unit_domain);
						auto basis_master=elem_master.evaluate_basis_values_at_points(slave_pts_unit_domain);
						//					
						
						
						for (int i = 0; i < n_basism; ++i){
							auto phi_m = basis_multiplier.get_function_view(i);
							loc_to_gbl_i=loc_to_gbl[i]; 
							for (int j = 0; j < n_basis0; ++j){
								auto phj_0 = basis_slave.get_function_view(j);
								for (int qp = 0; qp < n_qp; ++qp){
									out<<scalar_product(phi_m[qp],phj_0[qp])*w_meas*w_unit_domain[qp]*determinant<dim,dim>(grad_0[qp]);
									//					auto phi_0 = basis0.get_function_view(0);
									//					out<<phi_0[0]<<phi_0[1];
									//					auto phi_1 = basis1.get_function_view(0);
									//					out<<phi_1[0]<<phi_1[1];
									//					
									//					//		out<<"elem0"  <<to_string(elem_0_id) << endl;
								}
							}
							for (int j = 0; j < n_basis1; ++j){
								auto phj_0 = basis_master.get_function_view(j);
								for (int qp = 0; qp < n_qp; ++qp){
									out<<scalar_product(phi_m[qp],phj_0[qp])*w_meas*w_unit_domain[qp]*determinant<dim,dim>(grad_0[qp]);
									//					auto phi_0 = basis0.get_function_view(0);
									//					out<<phi_0[0]<<phi_0[1];
									//					auto phi_1 = basis1.get_function_view(0);
									//					out<<phi_1[0]<<phi_1[1];
									//					
									//					//		out<<"elem0"  <<to_string(elem_0_id) << endl;
								}
							}
							//					out<<"HERE";
						}
					}
				}
			}
		}
		elem_nb+=elem_nb;
	}
	//auto elem1=spaces[1]->get_element(1);
	////out << elem1.get_flat_index();
	
	return  0;
}
