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
 
/*!
* \file Customer.hpp
* \brief Class of the \c Customer.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright 
*
* This class represents a customer, or client, with its geographical coordinates and its costs of location w.r.t. to the two objectives in a FLP.
*
*/

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>

/*! \class Customer
* \brief Class to represent a \c Customer.
*
*  This class represents a \c Customer with all its attributes. The pair (x,y) representes respectively the first and second coordinates in a bi-dimensional geographical space.
*/
class Customer
{
public:
	/*!
	*	\brief Constructor of the class \c Customer.
	*/
	Customer();

	/*!
	*	\brief Getter for the demand.
	*	\return A double as the demand of this \c Customer.
	*/
	double getDemand() const;

	/*!
	*	\brief Setter for the demand.
	*	\param[in] val : A double which represents the demand of this \c Customer.
	*/
	void setDemand(double val);

private:
	double demand_;/*!< Double which represents the value of the demand of this \c Customer */
};

/*!
*	\relates Customer
*	\brief Operator overloading.
*
*	Overloading of the standard output stream in order to print a \c Customer.
*	\param[out] out : The standard output stream.
*	\param[in] cust : A \c Customer to print in the standard output stream.
*/
std::ostream& operator<<(std::ostream &out, const Customer *cust);

////////////////////////////////////////////////////////////////////////////////

// Inline functions

inline Customer::Customer() :
	demand_(0)
{
}

inline double Customer::getDemand() const
{
	return demand_;
}

inline void Customer::setDemand(double val)
{
	demand_ = val;
}

#endif
