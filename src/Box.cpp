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
 
#include "Box.hpp"
#include "Argument.hpp"
#include <limits>

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
	// TODO: p-objective
	//Calcul of the box
	for (unsigned int i = 0; i < data_.getnbCustomer(); ++i)
	{
		double vMinZ1 = DBL_MAX;
		double vMaxZ1 = -1;
		double vMinZ2 = DBL_MAX;
		double vMaxZ2 = -1;
		int iMinZ1 = -1, iMinZ2 = -1;
		for (unsigned int j = 0; j < data_.getnbFacility(); ++j)
		{
			//Search for local min and max
			if (facility_[j])
			{
				if ( data_.getAllocationObjCost(0, i, j) < vMinZ1 || (data_.getAllocationObjCost(0, i, j) == vMinZ1 && data_.getAllocationObjCost(1, i, j) < vMaxZ2) )
				{
					// <Z1 || =Z1 et <Z2
					vMinZ1 = data_.getAllocationObjCost(0, i, j);
					vMaxZ2 = data_.getAllocationObjCost(1, i, j);
					iMinZ1 = j;
				}
				if (data_.getAllocationObjCost(1, i, j) < vMinZ2 || (data_.getAllocationObjCost(1, i, j) == vMinZ2 && data_.getAllocationObjCost(0, i, j) < vMaxZ1) )
				{
					// <Z2 || =Z2 et <Z1
					vMinZ2 = data_.getAllocationObjCost(1, i, j);
					vMaxZ1 = data_.getAllocationObjCost(0, i, j);
					iMinZ2 = j;
				}
			}
		}
		//If they are equals, this allocation is optimal <=> "trivial"
		if (iMinZ1 == iMinZ2 && !Argument::capacitated)
		{
			minZ_[0] += data_.getAllocationObjCost(0, i, iMinZ1);
			maxZ_[0] += data_.getAllocationObjCost(0, i, iMinZ1);
			originZ_[0] += data_.getAllocationObjCost(0, i, iMinZ1);
			minZ_[1] += data_.getAllocationObjCost(1, i, iMinZ1);
			maxZ_[1] += data_.getAllocationObjCost(1, i, iMinZ1);
			originZ_[1] += data_.getAllocationObjCost(1, i, iMinZ1);
			nbCustomerNotAffected_--;
			isAssigned_[i] = true;
		}
		else
		//We add the lexicographically optimal cost
		{
			minZ_[0] += vMinZ1;
			minZ_[1] += vMinZ2;
			maxZ_[0] += vMaxZ1;
			maxZ_[1] += vMaxZ2;	
		}
	}
	if (nbCustomerNotAffected_ == 0)
	{
		//If all customers are affected, the box is a point, so there is no more WS step possible
		hasMoreStepWS_= false;
	}
}

bool Box::isFeasible() const
{
	// If problem is uncapacitated, it is always feasible
	if ( !Argument::capacitated ) return true;

	// Check if sum of demands is less or equal than sum of capacities
	double Qtotal( 0 ), dtotal( 0 );

	for ( unsigned int i = 0; i < data_.getnbCustomer(); ++i )
	{
		dtotal += data_.getCustomer(i).getDemand();
	}

	for ( unsigned int j = 0; j < data_.getnbFacility(); ++j )
	{
		if ( facility_[j] )
		{
			Qtotal += data_.getFacility(j).getCapacity();
		}
	}
	return dtotal <= Qtotal;
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
