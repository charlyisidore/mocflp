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
* \file LabelSetting.hpp
* \brief Class of the \c Box.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License 
*
* This class represents a \c LabelSetting which is the algorithm used to find all solutions.
*
*/

#ifndef LABELSETTING_H
#define LABELSETTING_H

#include <iostream>
#include <list>
#include <vector>

#include "Data.hpp"
#include "Box.hpp"
#include "Node.hpp"
#include "Solution.hpp"
#include "Functions.hpp"

extern bool modeVerbose;

/*! \class LabelSetting
* \brief Class to represent a \c LabelSetting.
*
*  This class represents a \c LabelSetting with all its attributes and methods.
*/
class LabelSetting
{
public:
    /*!
	*	\brief Default Constructor of the class \c LabelSetting.
	*
	*	\param[in] box : A \c Box object in which one ones runs the label setting algorithm.
	*/
    LabelSetting(Box &box);
    /*!
	*	\brief Destructor of the class \c LabelSetting.
	*/
    ~LabelSetting();
    
    /*!
	*	\brief A method to compute all solutions into a \c Box.
	*/
    void compute();
    /*!
	*	\brief A method to compute all the labels of the algorithm at level indexNode.
	*	\param indexNode : The level in which one ones executes the algorithm.
	*/
    void computeNode(int indexNode);
    /*!
	*	\brief A method to print informations about the label setting algorithm.
	*/
    void print();
    
    /*!
	*	\brief Getter for the rank (number of level).
	*	\return A unsigned int which value is the rank (number of level) of the algorithm.
	*/
    unsigned int getRank() const;
    
    Node *nodes_;/*!< A pointer of \c Node which represents all the solutions in each level */
    
private:
    unsigned int nbRank_;/*!< A unsigned int which represents the number of level of this algorithm in this \c Box */
    
    std::vector<double> boundZ_;/*!< A unsigned int which represents the maximum value w.r.t. objective k of this \c Box */
    
    Data &data_;/*!< A reference to the \c Data of this \c Box */
    
};
#endif
