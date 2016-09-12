/***************************************************************************
 *   Copyright (C) 2007 by Mauro Brenna, Ivan Reguzzoni   *
 *   malloblenne@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _VIRTUALORTOSCAN_H
#define _VIRTUALORTOSCAN_H

#include <vector>


//! This function emulate a scan from a laser range finder in a rectangular map delimited by xlow, ylow, xhigh, yhigh
/*!
 * \brief virtualortoscan
 * \param xr x coordinate of the robot
 * \param yr y coordinate of the robot
 * \param theta orientation of the robot in deg
 * \param xlow min x coordinate for rectangular map
 * \param ylow min y coordinate for rectangular map
 * \param xhigh max x coordinate for rectangular map
 * \param yhigh max y coordinate for rectangular map
 * \param xpt vector of points of the scan, x coordinates
 * \param ypt vector of points of the scan, y coordinates
 */
void virtualortoscan(float xr, float yr, float theta,float xlow, float ylow, float xhigh, float yhigh, vector<float>& xpt, vector<float>& ypt );

#endif /* _VIRTUALORTOSCAN_H */
