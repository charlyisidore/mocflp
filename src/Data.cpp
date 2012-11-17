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
 
#include "Data.hpp"

Data::Data(unsigned int nbFacility, unsigned int nbCustomer, const std::string & name)
{
	nbObjective_ = 2;

	// Allocate a matrix nbCustomer x nbFacility for each objective
	allocationObjCost_.resize( getNbObjective(),                   // p matrices
		std::vector< std::vector<double> > ( nbCustomer,       // with nbCustomer rows
		             std::vector<double>   ( nbFacility ) ) ); // and  nbFacility columns

	fileName_ = name;
	int index1 = int(fileName_.find('/'));
	int index2 = int(fileName_.find('.'));
	fileName_ = name.substr(index1 + 1, index2 - index1 - 1);
}

Data::~Data()
{
}

void Data::addFacility(const Facility & fac)
{
	facilityList_.push_back(fac);
}

void Data::addCustomer(const Customer & cust)
{
	customerList_.push_back(cust);
}

unsigned int Data::getnbFacility() const
{
	return facilityList_.size();
}

unsigned int Data::getnbCustomer() const
{
	return customerList_.size();
}

double Data::getAllocationObj1Cost(int cust, int fac) const
{
	return allocationObjCost_[0][cust][fac];
}

double Data::getAllocationObj2Cost(int cust, int fac) const
{
	return allocationObjCost_[1][cust][fac];
}

Facility & Data::getFacility(int fac)
{
	return facilityList_[fac];
}

const std::string & Data::getFileName() const
{
	return fileName_;
}

void Data::setAllocationObj1Cost(int cust,int fac, double val)
{
	allocationObjCost_[0][cust][fac] = val;
}

void Data::setAllocationObj2Cost(int cust,int fac, double val)
{
	allocationObjCost_[1][cust][fac] = val;
}

void Data::setFileName(const std::string & name)
{
	fileName_ = name;
}
