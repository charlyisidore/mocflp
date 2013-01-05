/*
 #License and Copyright
 
 #Version : 1.1
 
 #This file is part of BiUFLv2012.
 
 #BiUFLv2012 is Copyright © 2012, University of Nantes
 
 #BiUFLv2012 is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 
 #This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 #You should have received a copy of the GNU General Public License along with this program; if not, you can also find the GPL on the GNU web site.
 
 #In addition, we kindly ask you to acknowledge BiUFLv2012 and its authors in any program or publication in which you use BiUFLv2012. (You are not required to do so; it is up to your common sense to decide whether you want to comply with this request or not.) For general publications, we suggest referencing:  BiUFLv2012, MSc ORO, University of Nantes.
 
 #Non-free versions of BiUFLv2012 are available under terms different from those of the General Public License. (e.g. they do not require you to accompany any object code using BiUFLv2012 with the corresponding source code.) For these alternative terms you must purchase a license from Technology Transfer Office of the University of Nantes. Users interested in such a license should contact us (valorisation@univ-nantes.fr) for more information.
 */

// Macro constant which specializes the class Box for the bi-objective problem.
// Instead of using a dynamic std::vector to store values, it uses a static double C-style array.
// It speeds up the algorithm up to two times faster.
#define SPEEDUP_FOR_BI_OBJECTIVE 1

// Macro constant which specify if the class Box can use GLPK to compute the bounds.
// Useful if GLPK is not installed, else use of GLPK can be specified dynamically using option --mip.
#define CAN_USE_GLPK 1

#include "Box.hpp"
#include "Argument.hpp"
#include <limits>
#include <algorithm>

#if CAN_USE_GLPK
#include <glpk.h>
#endif

Box::Box(Data &data):
data_(data)
{
	//Set opening of depots to FALSE
	facility_.resize(data_.getnbFacility(), false);
	
	//Set allocation of customer to FALSE
	isAssigned_.resize(data_.getnbCustomer(), false);
    
	//All cost are 0 (nothing is allocated)
	nbCustomerNotAffected_ = data_.getnbCustomer();

	minZ_.resize(getNbObjective(), 0.);
	maxZ_.resize(getNbObjective(), 0.);
	originZ_.resize(getNbObjective(), 0.);
	id_ = "";
	totalCapacity_ = 0.;
}

Box::Box(Box* copy):
data_(copy->data_)
{
	//Set Facilities
	facility_ = copy->facility_;
	
	//Set Customers
	isAssigned_ = copy->isAssigned_;
	
	nbCustomerNotAffected_ = copy->nbCustomerNotAffected_;
	minZ_ = copy->minZ_;
	maxZ_ = copy->maxZ_;
	originZ_ = copy->originZ_;
	id_ = copy->id_;
	hasMoreStepWS_ = copy->hasMoreStepWS_;
	hasNeighbor_ = copy->hasNeighbor_;
	totalCapacity_ = 0.;
}

Box::Box(Data& data, const std::vector<bool> & toOpen):
data_(data)
{
	nbCustomerNotAffected_ = data_.getnbCustomer();

	minZ_.resize(getNbObjective(), 0.);
	maxZ_.resize(getNbObjective(), 0.);
	originZ_.resize(getNbObjective(), 0.);
	id_ = "";
	hasMoreStepWS_ = true; // A priori, some supported points exist
	hasNeighbor_ = false;
	totalCapacity_ = 0.;
	
	//Set opening of depots to FALSE
	facility_.resize(data_.getnbFacility(), false);
	for (unsigned int i = 0; i < data_.getnbFacility(); ++i)
	{
		if (toOpen[i])
		{
			openFacility(i);
			id_ += "1";
		}
		else
		{
			id_ += "0";
		}
	}
	//Set allocation of customer to FALSE
	isAssigned_.resize(data_.getnbCustomer(), false);
}

int Box::getNbFacilityOpen() const
{
	int ret = 0;
	for (unsigned int i = 0; i < data_.getnbFacility(); ++i)
	{
		if (isOpened(i))
		{
			++ret;
		}
	}
	return ret;
}

void Box::computeBox()
{
	if ( Argument::lagrangian )
	{
		computeBoxLagrangian();
	}
	else
#if CAN_USE_GLPK
	if ( Argument::mip_solver )
	{
		computeBoxMIP();
	}
	else
#endif
	if ( Argument::capacitated )
	{
		computeBoxHeuristic();
	}
	else
	{
		computeBoxUFLP();
	}

	if (nbCustomerNotAffected_ == 0)
	{
		// If all customers are affected, the box is a point, so there is no more WS step possible
		hasMoreStepWS_= false;
	}
}

// -----------------------------------------------------------------------------

void Box::computeBoxUFLP()
{
	// Computation of the box
	for (unsigned int i = 0; i < data_.getnbCustomer(); ++i)
	{
		// vLexOptZ[k][l] is an upper bound for the objective l among lexicographically optimal solutions w.r.t objective k.
		// vLexOptZ[k][k] is a lower bound for the objective k.

#if SPEEDUP_FOR_BI_OBJECTIVE
		// 2-objective
		double vLexOptZ[2][2] = {
			{ std::numeric_limits<double>::infinity(),
			  std::numeric_limits<double>::infinity() },
			{ std::numeric_limits<double>::infinity(),
			  std::numeric_limits<double>::infinity() }
		};
		int iMinZ[2] = { -1, -1 };
#else
		// p-objective
		std::vector< std::vector<double> > vLexOptZ( getNbObjective(),
			std::vector<double>( getNbObjective(), std::numeric_limits<double>::infinity() )
		);
		std::vector<int> iMinZ( getNbObjective(), -1 );
#endif

		// Enumerate open facilities
		for (unsigned int j = 0; j < data_.getnbFacility(); ++j)
		{
			// Search for local min and max
			if (facility_[j])
			{
				for ( int k = 0; k < getNbObjective(); ++k )
				{
					// If the point is a possible lexicographically optimal solution
					if ( data_.getAllocationObjCost(k, i, j) <= vLexOptZ[k][k] )
					{
						bool dominates( true );
						bool dominated( true );
						if ( data_.getAllocationObjCost(k, i, j) == vLexOptZ[k][k] )
						{
							for ( int l = 0; l < getNbObjective() && (dominates || dominated); ++l )
							{
								if ( vLexOptZ[k][l] < data_.getAllocationObjCost(l, i, j) )
								{
									dominates = false;
								}
								if ( data_.getAllocationObjCost(l, i, j) < vLexOptZ[k][l] )
								{
									dominated = false;
								}
							}
						}

						// If all objectives are better, replace all bounds
						// If at least one objective is better, update upper bounds
						if ( dominates || !dominated )
						{
							for ( int l = 0; l < getNbObjective(); ++l )
							{
								if ( dominates || data_.getAllocationObjCost(l, i, j) > vLexOptZ[k][l] )
								{
									vLexOptZ[k][l] = data_.getAllocationObjCost(l, i, j);
								}
							}
							iMinZ[k] = j;
						}
					}
				}
			}
		}

#if SPEEDUP_FOR_BI_OBJECTIVE
		if ( iMinZ[0] == iMinZ[1] )
#else
		if ( std::equal( iMinZ.begin(), iMinZ.end()-1, iMinZ.begin()+1 ) )
#endif
		{
			// If they are equals, this allocation is optimal <=> "trivial"
			for ( int k = 0; k < getNbObjective(); ++k )
			{
				double c( data_.getAllocationObjCost(k, i, iMinZ[0]) );
				minZ_[k]    += c;
				maxZ_[k]    += c;
				originZ_[k] += c;
			}
			--nbCustomerNotAffected_;
			isAssigned_[i] = true;
		}
		else
		{
			// We add the lexicographically optimal cost
			for ( int k = 0; k < getNbObjective(); ++k )
			{
				//      z2
				//      ^
				//      |
				//      |---[y1]----x           <-- vMax(z2)
				//      |           |
				//      |          [y3]
				//      |     [y2]  |
				//      |           |
				//      o---------------> z1
				//    z3
				//                  ^
				//                  |
				//                vMax(z1)
				//
				// The box must cover all lexicographically optimal points,
				// vMax is an upper bound w.r.t. objective k
				double vMaxZ( vLexOptZ[0][k] );
				for ( int l = 1; l < getNbObjective(); ++l )
				{
					if ( vMaxZ < vLexOptZ[l][k] )
						vMaxZ = vLexOptZ[l][k];
				}
				minZ_[k] += vLexOptZ[k][k];
				maxZ_[k] += vMaxZ;
			}
		}
	}
}

// -----------------------------------------------------------------------------

void Box::computeBoxHeuristic()
{
#if SPEEDUP_FOR_BI_OBJECTIVE
	// 2-objective
	double vLexOptZ[2][2] = { { 0, 0 }, { 0, 0 } };
#else
	// p-objective
	std::vector< std::vector<double> > vLexOptZ( getNbObjective(), std::vector<double>( getNbObjective(), 0. ) );
#endif

	for ( int k = 0; k < getNbObjective(); ++k )
	{
		// Remaining capacities
		std::vector<int> q( data_.getnbFacility() );
		for ( unsigned int j = 0; j < data_.getnbFacility(); ++j )
		{
			q[j] = data_.getFacility(j).getCapacity();
		}

		// To each customer we assign a facility
		for ( unsigned int i = 0; i < data_.getnbCustomer(); ++i )
		{
			double u, umax( -std::numeric_limits<double>::infinity() ),
			       vmin( std::numeric_limits<double>::infinity() );
			int jmin( -1 ), jmax( -1 );

			for ( unsigned int j = 0; j < data_.getnbFacility(); ++j )
			{
				if ( facility_[j] )
				{
					// Upper bound : find one solution (feasible if possible)
					u = ( q[j] - data_.getCustomer(i).getDemand() ) / data_.getAllocationObjCost(k, i, j);
					if ( u > umax )
					{
						jmax = j;
						umax = u;
					}

					// Lower bound : don't take capacities into account
					if ( data_.getAllocationObjCost(k, i, j) < vmin )
					{
						jmin = j;
						vmin = data_.getAllocationObjCost(k, i, j);
					}
				}
			}

			minZ_[k] += data_.getAllocationObjCost(k, i, jmin);

			// Add a penalty if solution is infeasible
			double penatly( ( q[jmax] < 0 ) ? 1.5 : 1. );

			for ( int l = 0; l < getNbObjective(); ++l )
			{
				vLexOptZ[k][l] += penatly * data_.getAllocationObjCost(l, i, jmax);
			}
			q[jmax] -= data_.getCustomer(i).getDemand();
		}
	}

	// We add the lexicographically optimal cost
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		// The box must cover all lexicographically optimal points,
		// vMax is an upper bound w.r.t. objective k
		double vMaxZ( vLexOptZ[0][k] );
		for ( int l = 1; l < getNbObjective(); ++l )
		{
			if ( vMaxZ < vLexOptZ[l][k] )
				vMaxZ = vLexOptZ[l][k];
		}
		maxZ_[k] += vMaxZ;
	}
}

// -----------------------------------------------------------------------------

void Box::computeBoxLagrangian()
{
}

// -----------------------------------------------------------------------------

void Box::computeBoxMIP()
{
#if CAN_USE_GLPK
#if SPEEDUP_FOR_BI_OBJECTIVE
	// 2-objective
	double vLexOptZ[2][2] = { { 0, 0 }, { 0, 0 } };
#else
	// p-objective
	std::vector< std::vector<double> > vLexOptZ( getNbObjective(), std::vector<double>( getNbObjective(), 0. ) );
#endif

	if ( Argument::verbose )
	{
		std::clog << "MIP solving..." << std::endl;
	}

	// Indices of open facilities
	std::vector<int> fac;

	for ( unsigned int j = 0; j < data_.getnbFacility(); ++j )
	{
		if ( facility_[j] )
		{
			fac.push_back( j );
		}
	}

	int m( data_.getnbCustomer() );
	int n( fac.size() );

	int n_cols( m*n );
	int n_rows( Argument::capacitated ? m+n : m );

	// CREATE GLPK PROBLEM

	int ne = Argument::capacitated ? 2*m*n : m*n;
	int ia[ne];
	int ja[ne];
	double ar[ne];

	glp_prob * lp = glp_create_prob();

	glp_set_obj_dir( lp, GLP_MIN );
	glp_add_rows( lp, n_rows );
	glp_add_cols( lp, n_cols );

	// SET FIXED COEFFICIENTS AND BOUNDS

	// Set variables
	for ( int i = 0; i < n_cols; ++i )
	{
		glp_set_col_kind( lp, 1 + i, GLP_BV );
	}

	// Set assignment constraints
	for ( int i = 0; i < m; ++i )
	{
		glp_set_row_bnds( lp, 1 + i, GLP_FX, 1, 1 );
		//glp_set_row_bnds( lp, 1 + i, GLP_LO, 1, 1 );

		for ( int j = 0; j < n; ++j )
		{
			int index = 1 + i*n + j;

			ia[index] = 1 + i;       // Row in [1,m]
			ja[index] = 1 + i*n + j; // Col in [i,i+n]
			ar[index] = 1;
		}
	}

	// Set capacity constraints
	if ( Argument::capacitated )
	{
		for ( int j = 0; j < n; ++j )
		{
			glp_set_row_bnds( lp, 1 + m+j, GLP_UP, 0, data_.getFacility( fac[j] ).getCapacity() );

			for ( int i = 0; i < m; ++i )
			{
				int index = m*n + (1 + i*n + j);

				ia[index] = 1 + m+j;     // Row in [m+1,m+1+n]
				ja[index] = 1 + i*n + j; // Col in [i,i+n]
				ar[index] = data_.getCustomer(i).getDemand();
			}
		}
	}

	// Load all constraints and variables
	glp_load_matrix( lp, ne, ia, ja, ar );

	// PARAMETERS

	glp_smcp scmp_parm;
	glp_iocp iocp_parm;

	glp_init_smcp( &scmp_parm );
	glp_init_iocp( &iocp_parm );

	scmp_parm.msg_lev  = GLP_MSG_ERR;
	scmp_parm.presolve = GLP_ON;

	iocp_parm.msg_lev  = GLP_MSG_ERR;
	iocp_parm.presolve = GLP_ON;

	// SOLVE

	for ( int k = 0; k < getNbObjective(); ++k )
	{
		// Set objective function k
		for ( int i = 0; i < m; ++i )
		{
			for ( int j = 0; j < n; ++j )
			{
				int index = 1 + i*n + j;

				// Additional cost to try to avoid weakly efficient solutions
				double c( 0 );
				for ( int l = 0; l < getNbObjective(); ++l )
				{
					if ( k != l )
						c += data_.getAllocationObjCost( l, i, fac[j] );
				}
				c = data_.getAllocationObjCost( k, i, fac[j] ) + c * std::numeric_limits<double>::epsilon();

				glp_set_obj_coef( lp, index, c );
			}
		}

		// Solve using simplex and branch & bound
		int res = glp_simplex( lp, &scmp_parm );
		res = glp_intopt( lp, &iocp_parm );

		if ( res == 0 )
		{
			int status = glp_mip_status( lp );
			if ( status == GLP_OPT )
			{
				// Compute objective values of objectives != k
				for ( int l = 0; l < getNbObjective(); ++l )
				{
					for ( int i = 0; i < m; ++i )
					{
						for ( int j = 0; j < n; ++j )
						{
							int index = 1 + i*n + j;
							vLexOptZ[k][l] +=
								  glp_mip_col_val( lp, index )
								* data_.getAllocationObjCost( l, i, fac[j] );
						}
					}
				}
			}
			else
			{
				std::cerr << "[GLPK Error] Resolution is not optimal. Code: ";
				switch ( status )
				{
					case GLP_UNDEF:  std::cerr << "GLP_UNDEF" << std::endl; break;
					case GLP_FEAS:   std::cerr << "GLP_FEAS" << std::endl; break;
					case GLP_NOFEAS:  std::cerr << "GLP_NOFEAS" << std::endl; break;
					default: std::cerr << "unknown (" << status << ")" << std::endl; break;
				}
			}
		}
		else
		{
			std::cerr << "[GLPK Error] Resolution has failed. Code: ";
			switch ( res )
			{
				case GLP_EBOUND:  std::cerr << "GLP_EBOUND" << std::endl; break;
				case GLP_EROOT:   std::cerr << "GLP_EROOT" << std::endl; break;
				case GLP_ENOPFS:  std::cerr << "GLP_ENOPFS" << std::endl; break;
				case GLP_ENODFS:  std::cerr << "GLP_ENODFS" << std::endl; break;
				case GLP_EFAIL:   std::cerr << "GLP_EFAIL" << std::endl; break;
				case GLP_EMIPGAP: std::cerr << "GLP_EMIPGAP" << std::endl; break;
				case GLP_ETMLIM:  std::cerr << "GLP_ETMLIM" << std::endl; break;
				case GLP_ESTOP:   std::cerr << "GLP_ESTOP" << std::endl; break;
				default: std::cerr << "unknown (" << res << ")" << std::endl; break;
			}
		}
	}

	// FREE MEMORY
	glp_delete_prob( lp );

	// COMPUTE BOX BOUNDS

	for ( int k = 0; k < getNbObjective(); ++k )
	{
		double vMaxZ( vLexOptZ[0][k] );
		for ( int l = 1; l < getNbObjective(); ++l )
		{
			if ( vMaxZ < vLexOptZ[l][k] )
				vMaxZ = vLexOptZ[l][k];
		}

		minZ_[k] += vLexOptZ[k][k];
		maxZ_[k] += vMaxZ;
	}
#endif
// CAN_USE_GLPK
}

// -----------------------------------------------------------------------------

bool Box::isFeasible() const
{
	// If problem is uncapacitated, it is always feasible
	if ( !Argument::capacitated ) return true;

	// Check if sum of demands is less or equal than sum of capacities
	double dtotal( 0 );

	for ( unsigned int i = 0; i < data_.getnbCustomer(); ++i )
	{
		dtotal += data_.getCustomer(i).getDemand();
	}

	return totalCapacity_ >= dtotal;
}

void Box::openFacility(int fac)
{
	facility_[fac] = true;

	// We add costs to the box
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		double c = data_.getFacility(fac).getLocationObjCost(k);
		minZ_[k] += c;
		maxZ_[k] += c;
		originZ_[k] += c;
	}

	// Add capacity
	totalCapacity_ += data_.getFacility(fac).getCapacity();
}

void Box::print()
{
#ifdef verbose
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		std::cout << "minZ" << k+1 << " =" << getMinZ(k) << std::endl;
	}
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		std::cout << "maxZ" << k+1 << " =" << getMaxZ(k) << std::endl;
	}
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		std::cout << "originZ" << k+1 << " =" << getOriginZ(k) << std::endl;
	}
	for ( unsigned int i = 0; i < data_.getnbCustomer(); ++i )
	{
		if ( !isAssigned_[i] )
		{
			std::cout << "Customer: " << i << std::endl;
			for ( int k = 0; k < getNbObjective(); ++k )
			{
				for ( unsigned int j = 0; j < data_.getnbFacility(); ++j )
				{
					if ( isOpened(j) )
					{
						std::cout << data_.getAllocationObjCost(k,i,j) << "-";
					}
				}
				std::cout << std::endl;
			}
		}
	}
#endif
}

//***** PUBLIC FONCTIONS *****

bool isDominatedBetweenTwoBoxes(Box *box1, Box *box2)
{
	// | +---+          |
	// | | 1 |          |
	// | o---+          | +---+
	// o---+-----   +---+ | 1 |
	// | 2 |        | 2 | o---+
	// +---+        +---o---------

	bool dominatedBy1Obj = false;
	for ( int k = 0; k < box1->getNbObjective(); ++k )
	{
		if ( !( box2->getMinZ(k) < box1->getMinZ(k) ) ) return false;
		if ( !dominatedBy1Obj && ( box2->getMaxZ(k) < box1->getMinZ(k) ) ) dominatedBy1Obj = true;
	}
	return dominatedBy1Obj;
}

bool isDominatedBetweenOrigins(Box *box1, Box *box2)
{
	// |                |
	// |                |
	// | o              |
	// o---+-----   +---+
	// | 2 |        | 2 | o
	// +---+        +---o---------

	bool dominatedBy1Obj = false;
	for ( int k = 0; k < box1->getNbObjective(); ++k )
	{
		if ( !( box2->getMinZ(k) < box1->getOriginZ(k) ) ) return false;
		if ( !dominatedBy1Obj && ( box2->getMaxZ(k) < box1->getOriginZ(k) ) ) dominatedBy1Obj = true;
	}
	return dominatedBy1Obj;
}

void filterDominatedBoxes(std::vector<Box*> &vectBox)
{
	for ( unsigned int it = 0; it < vectBox.size(); ++it )
	{
		for ( unsigned int it2 = it + 1; it2 < vectBox.size(); ++it2 )
		{
			if ( isDominatedBetweenTwoBoxes( vectBox[it2] , vectBox[it] ) )
			{
				//it2 is dominated
				delete (vectBox[it2]);
				vectBox.erase((vectBox.begin())+=it2);
				--it2;//We delete it2, so we shift by one
			}
			else
			{
				if ( isDominatedBetweenTwoBoxes( vectBox[it] , vectBox[it2] ) )
				{
					//it is dominated
					delete (vectBox[it]);
					vectBox.erase((vectBox.begin())+=it);
					//Delete it, pass to the next loop. Initialize it2 to it+1
					it2 = it;
				}
			}
		}
	}
}

bool isDominatedByItsOrigin(std::vector<Box*> &vectBox, Box *box)
{
	bool isDominated(false);
	std::vector<Box*>::iterator it;
	/*Stop Criterion
	- all the vector is travelled (but condition 3 may occur before)
	- we are dominated
	- we compare with itself
	*/
	for ( it = vectBox.begin(); it != vectBox.end() && !isDominated && (*it != box); ++it )
	{
		if ( isDominatedBetweenOrigins( box , (*it) ) )
		{
			isDominated = true;
		}
  	}
	return isDominated;
}

bool isDominatedByItsBox(std::vector<Box*> &vectBox, Box *box)
{
	bool isDominated(false);
	std::vector<Box*>::iterator it;
	/*Stop Criterion
	- all the vector is travelled (but condition 3 may occur before)
	- we are dominated
	- we compare with itself
	*/
	for ( it = vectBox.begin(); it != vectBox.end() && !isDominated && (*it != box); ++it )
	{
		if ( isDominatedBetweenTwoBoxes( box , (*it) ) )
		{
			isDominated = true;
		}
	}	
	return isDominated;
}
