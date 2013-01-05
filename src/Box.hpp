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
* \file Box.hpp
* \brief Class of the \c Box.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License
*
* This class represents an object \c Box. A \c Box is a sub-space of dimension 2 defined in the objective space and characterized by at most two feasible solutions. These solutions correspond to the two lexicographic optimal solutions for the two objective functions when a set of \c Facility opened is considered.
*
*/
#ifndef BOX_H
#define BOX_H

#include <iostream>
#include <vector>
#include <string>
#include <cfloat>

#include "Data.hpp"

/*! \class Box
* \brief Class to represent a \c Box.
*
*  This class represents a \c Box with all its attributes and methods.
*/
class Box
{
public:
	/*!
	*	\brief Default Constructor of the class \c Box.
	*
	*	The default construtor gives a \c Box which anyone \c Facility opened.
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*/
	Box(Data &data);

	/*!
	*	\brief Constructor of copy of the class \c Box.
	*
	*	\param[in] copy : A \c Box to copy.
	*/
	Box(Box* copy);

	/*!
	*	\brief Constructor of the class \c Box.
	*
	*	This construtor gives a \c Box which a set of \c Facilities opened.
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*	\param[in] toOpen : A vector of boolean representing the vector of \c Facility to open in order to construct an object \c Box.
	*/
	Box(Data &data, const std::vector<bool> & toOpen);

	/*!
	*	\brief Getter for the number of objectives.
	*	\return A int as the number of objectives.
	*/
	int getNbObjective() const;

	/*!
	*	\brief Getter for the minimum value w.r.t. objective k.
	*	\param[in] k : The index of the objective.
	*	\return A double as the minimum value w.r.t. objective k of this \c Box.
	*/
	double getMinZ(int k) const;

	/*!
	*	\brief Getter for the maximum value w.r.t. objective k.
	*	\param[in] k : The index of the objective.
	*	\return A double as the maximum value w.r.t. objective k of this \c Box.
	*/
	double getMaxZ(int k) const;

	/*!
	*	\brief Getter for the value of the origin w.r.t. objective k.
	*	\param[in] k : The index of the objective.
	*	\return A double as the value of the point of origin w.r.t. objective k of this \c Box.
	*/
	double getOriginZ(int k) const;

	/*!
	*	\brief Setter for the minimum value w.r.t. objective k.
	*	\param[in] k : The index of the objective.
	*	\param[in] v : A double which represents the minimum value w.r.t. objective k of this \c Box.
	*/
	void setMinZ(int k, double v);

	/*!
	*	\brief Setter for the maximum value w.r.t. objective k.
	*	\param[in] k : The index of the objective.
	*	\param[in] v : A double which represents the maximum value w.r.t. objective k of this \c Box.
	*/
	void setMaxZ(int k, double v);

	/*!
	*	\brief Getter for the id.
	*	\return A string as the sequence of 1 and 0 representing the combination of \c Facility of this \c Box.
	*/
	const std::string & getId() const;

	/*!
	*	\brief method to know if a \c Customer is assigned or not.
	*	\param[in] cust : A \c Customer.
	*	\return A boolean which value is TRUE if the \c Customer is assigned to any \c Facility.
	*/
	bool isAssigned(int cust) const;

	/*!
	*	\brief method to know if a \c Facility is opened or not.
	*	\param[in] cust : A \c Facility.
	*	\return A boolean which value is TRUE if the \c Facility is opened.
	*/
	bool isOpened(int fac) const;

	/*!
	*	\brief Getter for the number of \c Customers nonaffected.
	*	\return An int as the number of \c Customers which are not affected to a \c Facility.
	*/
	int getnbCustomerNotAffected() const;

	/*!
	*	\brief Getter for the number of \c Facilities opened.
	*	\return An int as the number of \c Facilities which are opened (set to 1).
	*/
	int getNbFacilityOpen() const;

	/*!
	*	\brief Getter for the neighborhood.
	*	\return A boolean which value is TRUE if this \c Box overlaps or is overlapped with an other \c Box.
	*/
	bool getHasNeighbor() const;

	/*!
	*	\brief Getter for the number of Weighted Sum.
	*	\return A boolean if this \c Box gets a remaining iteration of weigthed sum method.
	*/
	bool getHasMoreStepWS() const;

	/*!
	*	\brief Getter for the data.
	*	\return A reference \c Data of the \c Data of this \c Box.
	*/
	Data &getData() const;

	/*!
	*	\brief Setter for the id of this \c Box.
	*	\param[in] s : A string which represents the id (combination of \c Facility) of this \c Box.
	*/
	void setId(const std::string & s);

	/*!
	*	\brief Setter for the neighborhood.
	*	\param[in] b : A boolean which value is TRUE if this \c Box has at least one neighborhood (a \c Box which overlaps or is overlapped).
	*/
	void setHasNeighbor(bool b);

	/*!
	*	\brief Setter for the remaining weighted sum method.
	*	\param[in] b : A boolean which value is TRUE if this \c Box has a remaining iteration of weighted sum method.
	*/
	void setHasMoreStepWS(bool b);

	/*!
	*	\brief A method to expand a box, which means attempting to allocate all \c Customers to \c Facilities.
	*/
	void computeBox();

	/*!
	*	\brief A method to know if a \c Box can be feasible or not (check constraints).
	*	\return A boolean which value is TRUE if the \c Box can be feasible.
	*/
	bool isFeasible() const;

	/*!
	*	\brief A method that opens a \c Facility in this \c Box, by adding all the location cost of the two objectives.
	*	\param[in] fac : A \c Facility to open.
	*/
	void openFacility(int fac);

	/*!
	*	\brief A method to print informations about this \c Box.
	*/
	void print();

private:
	/*!
	*	\brief Compute the box efficiently for UFLP.
	*/
	void computeBoxUFLP();

	/*!
	*	\brief Compute the box using an heuristic.
	*/
	void computeBoxHeuristic();

	/*!
	*	\brief Compute the box using a lagrangian relaxation algorithm.
	*/
	void computeBoxLagrangian();

	/*!
	*	\brief Compute the box using a mono-objective MIP solver.
	*/
	void computeBoxMIP();

	std::string id_;/*!< A string which represents the id of this \c Box */

	Data& data_;/*!< A reference to the \c Data of this \c Box */

	std::vector<bool> isAssigned_;/*!< A boolean which represents the vector of \c Customer assigned or not */
	std::vector<bool> facility_;/*!< A boolean which represents the vector of \c Facility opened or not */
	bool hasMoreStepWS_;/*!< A boolean which represents if this \c Box has a remaining iteration of weighted sum method*/
	bool hasNeighbor_;/*!< A boolean which represents if this \c Box has a neighboor or not */

	int nbCustomerNotAffected_; /*!< An integer which represents the number of \c Customer not affected of this \c Box*/

	double totalCapacity_;/*!< A double which represents total capacities of all open facilities this \c Box */

	std::vector<double> minZ_;/*!< A double which represents the minimum value w.r.t. objective k*/
	std::vector<double> maxZ_;/*!< A double which represents the maximum value w.r.t. objective k*/
	std::vector<double> originZ_;/*!< A double which represents the value of the origin of this \c Box w.r.t. objective k */
};

/*!
* 	\relates Box
*	\brief Method of comparison between two boxes.
*
*	A method to compare two \c Boxes using the bounds minZ1_, minZ2_, maxZ1_, maxZ2_ of each \c Boxes.
*	\param[in] box1 : A \c Box to compare.
*	\param[in] box2 : A \c Box to compare.
*	\return A boolean which value is TRUE if the box1 is dominated by the box2. 
*/
bool isDominatedBetweenTwoBoxes(Box *box1, Box *box2);
/*!
* 	\relates Box
*	\brief Method of comparison between two boxes.
*
*	A method to compare two \c Boxes using the bounds minZ1_, minZ2_, maxZ1_, maxZ2_, originZ1_ and originZ2_.
*	\param[in] box1 : A \c Box to compare.
*	\param[in] box2 : A \c Box to compare.
*	\return A boolean which value is TRUE if the origin of the box1 is dominated by the box2. 
*/
bool isDominatedBetweenOrigins(Box *box1, Box *box2);
/*!
* 	\relates Box
*	\brief Method to filter a vector of \c Boxes
*
*	A method to filter and delete \c Box in a vector of \c Boxes by comparing each other. 
*	\param[in] vectBox : A vector of \c Boxes.
*/
void filterDominatedBoxes(std::vector<Box*> &vectBox);
/*!
* 	\relates Box
*	\brief Method of comparison between a \c Box and a vector of \c Boxes.
*
*	\param[in] vectBox : A vector of \c Boxes to compare.
*	\param[in] box : A \c Box to compare.
*	\return A boolean which value is TRUE if \c Box box is dominated by one of the \c Box of the vector vectBox. 
*/
bool isDominatedByItsOrigin(std::vector<Box*> &vectBox, Box *box);
/*!
* 	\relates Box
*	\brief Method of comparison between a \c Box and a vector of \c Boxes.
*
*	\param[in] vectBox : A vector of \c Boxes to compare.
*	\param[in] box : A \c Box to compare.
*	\return A boolean which value is TRUE if one of the \c Box of the vector vectBox is dominated by the \c Box box. 
*/
bool isDominatedByItsBox(std::vector<Box*> &vectBox, Box *box);

////////////////////////////////////////////////////////////////////////////////

// Inline functions

inline int Box::getNbObjective() const
{
	return data_.getNbObjective();
}

inline double Box::getMinZ(int k) const
{
	return minZ_[k];
}

inline double Box::getMaxZ(int k) const
{
	return maxZ_[k];
}

inline double Box::getOriginZ(int k) const
{
	return originZ_[k];
}

inline void Box::setMinZ(int k, double v)
{
	minZ_[k] = v;
}

inline void Box::setMaxZ(int k, double v)
{
	maxZ_[k] = v;
}

inline const std::string & Box::getId() const
{
	return id_;
}

inline bool Box::isAssigned(int cust) const
{
	return isAssigned_[cust];
}

inline bool Box::isOpened(int fac) const
{
	return facility_[fac];
}

inline int Box::getnbCustomerNotAffected() const
{
	return nbCustomerNotAffected_;
}

inline bool Box::getHasNeighbor() const
{
	return hasNeighbor_;
}

inline bool Box::getHasMoreStepWS() const
{
	return hasMoreStepWS_;
}

inline Data& Box::getData() const
{
	return data_;
}

inline void Box::setId(const std::string & s)
{
	id_ += s;
}

inline void Box::setHasNeighbor(bool b)
{
	hasNeighbor_ = b;
}

inline void Box::setHasMoreStepWS(bool b)
{
	hasMoreStepWS_ = b;
}

#endif
