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
* \file Node.hpp
* \brief Class of the \c Node.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License 
*
* This class represents a \c Node which represents a level in the Label Setting algorithm.
*
*/

#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <list>

#include "Solution.hpp"

/*! \namespace std
* 
* Using the standard namespace std of the IOstream library of C++.
*/
using namespace std;

extern bool modeVerbose;

/*! \class Node
* \brief Class to represent a \c Node.
*
*  This class represents a \c Node with all its attributes and methods.
*/
class Node
{
public:
    /*!
	*	\brief Default Constructor of the class \c Node.
	*/
    Node();
    /*!
	*	\brief Default Constructor of the class \c Node.
	*
	*	\param[in] size : An int which represents the size of this node.
	*/
    Node(int size);
    /*!
	*	\brief Destructor of the class \c Node.
	*/
    ~Node();
    
    /*!
	*	\brief Setter for the size of this \c Node.
	*	\param[in] s : A unsigned int which represents size of this \c Node.
	*/
    void setSize(unsigned int s);
    
    /*!
	*	\brief Getter for the size.
	*	\return An unsigned int as the size of this \c Node.
	*/
    unsigned int getSize() const;
    /*!
	*	\brief Getter for the cost to enter to this \c Node w.r.t. objective 1.
	*	\param i : The index.
	*	\return A double as the cost to enter to this \c Node w.r.t. objective 1 at the index i.
	*/
    double getCostToEnterZ1(int i) const;
    /*!
	*	\brief Getter for the cost to enter to this \c Node w.r.t. objective 2.
	*	\param i : The index.
	*	\return A double as the cost to enter to this \c Node w.r.t. objective 2 at the index i.
	*/
    double getCostToEnterZ2(int i) const;
    
    /*!
	*	\brief A method to delete all the labels of this \c Node.
	*/
    void clearLabels();
     /*!
	*	\brief Setter of the values of the solution ones want to set.
	*	\param[in] index : The index.
	*	\param[in] z1 : The value of the solution w.r.t. objective 1 ones wants to set.
	*	\param[in] z2 : The value of the solution w.r.t. objective 2 ones wants to set.
	*/
    void setValues(int index, double z1, double z2);
    /*!
	*	\brief A method to print informations about this \c Node.
	*/
    void print();
    
    list<Solution> labels_;/*!< A list of \c Solution which represents all the \c Solutions (or labels) in this \c Node*/
	
private:
    unsigned int size_;/*!< A unsigned int which represents the size of this \c Node*/
    
    double *costToEnterZ1_;/*!< A pointer of double which represents all the values to enter to this \c Node w.r.t objective 1*/
    double *costToEnterZ2_;/*!< A pointer of double which represents all the values to enter to this \c Node w.r.t objective 2*/
};

#endif
