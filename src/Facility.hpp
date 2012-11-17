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
* \file Facility.hpp
* \brief Class of the \c Facility.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License 
*
* This class represents a facility, or warehouse or service, with its geographical coordinates and its costs of location w.r.t. to the two objectives in a FLP.
*
*/

#ifndef FACILITY_H
#define FACILITY_H

#include <iostream>
#include <vector>

/*! \class Facility
* \brief Class to represent a \c Facility.
*
*  This class represents a \c Facility with all its attributes. The pair (x,y) representes respectively the first and second coordinates in a bi-dimensional geographical space.
*/
class Facility
{
public:
    /*!
	*	\brief Constructor of the class \c Facility.
	*
	*	\param[in] x : An unsigned integer which represents the x coordinate of the \c Facility.
	*	\param[in] y : An unsigned integer which represents the y coordinate of the \c Facility.
	*/
    Facility(unsigned short x, unsigned short y);

	/*!
	*	\brief Getter for the number of objectives.
	*	\return A int as the number of objectives.
	*/
    int getNbObjective() const;
    /*!
	*	\brief Getter for the location cost w.r.t. objective k.
	*	\return A double as the location cost w.r.t. objective k of this \c Facility.
	*/
    double getLocationObjCost(int k) const;
    /*!
	*	\brief Setter for the location cost w.r.t. objective k.
	*	\param[in] val : A double which represents the value of the location cost of this \c Facility w.r.t. objective k.
	*/
    void setLocationObjCost(int k, double val);
   	/*!
	*	\brief Getter for the x coordinate.
	*	\return An unsigned short as the x coordinate of this \c Facility.
	*/
    unsigned short getCoordX() const;
    /*!
	*	\brief Getter for the y coordinate.
	*	\return An unsigned short as the y coordinate of this \c Facility.
	*/
    unsigned short getCoordY() const;
    /*!
	*	\deprecated
	*	\brief Getter for the location cost w.r.t. objective 1.
	*	\return A double as the location cost w.r.t. objective 1 of this \c Facility.
	*/
    double getLocationObj1Cost() const;
    /*!
	*	\deprecated
	*	\brief Getter for the location cost w.r.t. objective 2.
	*	\return A double as the location cost w.r.t. objective 2 of this \c Facility.
	*/
    double getLocationObj2Cost() const;
    
    /*!
	*	\deprecated
	*	\brief Setter for the location cost w.r.t. objective 1.
	*	\param[in] val : A double which represents the value of the location cost of this \c Facility w.r.t. objective 1.
	*/
    void setLocationObj1Cost(double val);
    /*!
	*	\deprecated
	*	\brief Setter for the location cost w.r.t. objective 2.
	*	\param[in] val : A double which represents the value of the location cost of this \c Facility w.r.t. objective 2.
	*/
    void setLocationObj2Cost(double val);
    
private:
    unsigned short coordX_;/*!< Unsigned short which represents the value of the x coordinate of this \c Facility */
    unsigned short coordY_;/*!< Unsigned short which represents the value of the y coordinate of this \c Facility */
    std::vector<double> locationObjCost_;/*!< Double which represents the value of the location cost of this \c Facility w.r.t. objective k */
};

/*!
* 	\relates Facility
*	\brief Operator overloading.
*
*	Overloading of the standard output stream in order to print a \c Facility.
*	\param[out] out : The standard output stream.
*	\param[in] fac : A \c Facility to print in the standard output stream.
*/
std::ostream& operator<<(std::ostream &out, const Facility *fac);

////////////////////////////////////////////////////////////////////////////////

// Inline functions

inline int Facility::getNbObjective() const
{
	return locationObjCost_.size();
}

inline double Facility::getLocationObjCost(int k) const
{
    return locationObjCost_[k];
}

inline void Facility::setLocationObjCost(int k, double val)
{
    locationObjCost_[k] = val;
}

#endif
