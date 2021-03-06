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
* \file Solution.hpp
* \brief Class of the \c Solution.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright 
*
* This class represents a solution. In FLP, a solution is representes by two values respectively for the objective 1 and the objective 2.
*
*/

#ifndef SOLUTION_H
#define SOLUTION_H

// Macro constant which specializes the class Solution for the bi-objective problem.
// Instead of using a dynamic std::vector to store values, it uses a static double C-style array.
// It speeds up the algorithm up to two times faster.
#define SPEEDUP_FOR_BI_OBJECTIVE 1

#include <vector>

/*! \class Solution
* \brief Class to represent a \c Solution.
*
*  This class represents a \c Solution with its array attribute (obj_).
*/
class Solution
{
public:
	/*!
	*	\brief Default constructor of the class \c Solution.
	*
	*	\param[in] nbObjective : The number of objectives.
	*/
	Solution(int nbObjective);

	/*!
	*	\brief Getter for the number of objectives.
	*	\return A int as the number of objectives.
	*/
	int getNbObjective() const;

	/*!
	*	\brief Getter for the value w.r.t. objective k of this \c Solution.
	*	\param[in] k : The index of the objective.
	*	\return A double as the value w.r.t. objective k of this \c Solution.
	*/
	double getObj(int k) const;

	/*!
	*	\brief Setter for the value w.r.t objective k of this \c Solution.
	*	\param[in] k : The index of the objective.
	*	\param[in] obj : A double which represents the value of the \c Solution w.r.t objective k.
	*/
	void setObj(int k, double obj);

private:
#if SPEEDUP_FOR_BI_OBJECTIVE
	double obj_[2]; /*!< Static double C-style array which represents the value w.r.t. objective k of this \c Solution */
#else
	std::vector<double> obj_; /*!< Dynamic double vector which represents the value w.r.t. objective k of this \c Solution */
#endif
};

/*!
*	\relates Solution
*	\brief Operator overloading.
*
*	Overloading of the comparison operator \c < in order to compare two solutions.
*	\param[in] s1 : The first \c Solution to compare.
*	\param[in] s2 : The second \c Solution to compare.
*	\return A boolean which gets \c TRUE if the value w.r.t. objective 1 of the \c Solution s1 is stricty lower to the value w.r.t. objective 1 of the \c Solution s2, FALSE otherwise.
*/
bool operator< (const Solution & s1, const Solution & s2);

////////////////////////////////////////////////////////////////////////////////

// Inline functions

inline Solution::Solution(int nbObjective)
#if !SPEEDUP_FOR_BI_OBJECTIVE
 : obj_(nbObjective)
#endif
{
}

inline int Solution::getNbObjective() const
{
#if SPEEDUP_FOR_BI_OBJECTIVE
	return 2;
#else
	return obj_.size();
#endif
}

inline double Solution::getObj(int k) const
{
	return obj_[k];
}

inline void Solution::setObj(int k, double obj)
{
	obj_[k] = obj;
}

inline bool operator< (const Solution & s1, const Solution & s2)
{
	return s1.getObj(0) < s2.getObj(0);
}

#endif
