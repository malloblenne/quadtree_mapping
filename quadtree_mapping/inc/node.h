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
#ifndef _NODE_H
#define _NODE_H
#include <iostream>

typedef unsigned int uint;

namespace QuadtreeLib
{
//! Containing the definitions of the 4 Directions
namespace Direction
{
//! C++<11 enum for directions
enum e
{
    NW = 0,
    NE = 1,
    SW = 2,
    SE = 3
};

}

//! Cartesian coordinates, 2d
namespace Axis
{
//! Definition of the enum
 enum e
 {
     X = 0,
     Y = 1
 };
}


//! Class dealing with a single node of the quadtree. It can be a leaf node or contains sub-quadtree
class Node
{
    //! Utility function for printing purposes
    //! \param output stream in output
    //! \param node node to be streamed
    //! \return stream in output
    friend std::ostream &operator<<(std::ostream & output, Node & node);

public:

    //! Constructor
    Node(bool l,unsigned int v,unsigned int oc, unsigned int d, int x, int y);

    //! Destructor
    virtual ~Node();

    //! Insert new point
    bool insertPt (const float&xr,const float &yr, const float &xpt, const float &ypt);

    //! Getter for dimension
    //! \return dimension
    unsigned int getDim() const;

    //! Getter if is leaf
    //! \return true if node is leaf
    bool isLeaf() const;

    //! Get X corner
    //! \return x corner
    int getCornX() const;

    //! Get Y corner
    //! \return y corner
    int getCornY() const;

    //! Set node as a child
    //! \param direction direction to be assigned
    //! \param n child node
    void setChild(Direction::e direction,Node *n);

    Node* getChild(Direction::e direction) const;

    void getNewCorner(int array[4][2]);

    float occProbability() const;

    void incVis(int inc=1);

    void incOcc(int inc=1);

private:

    Node* intUpdate (const float &xr, const float &yr, const float &xpt, const float &ypt);

    //! if leaf=true else branch
    bool leaf;

    //! Children forming sub-quadtree, or NULL or 4. Directions: NW=0 NE=1 SW=2 SE=3
    Node *children[4];

    //! Visited
    unsigned int vis;

    //! Occupied
    unsigned int occ;

    //! Edge dimension (not area)
    unsigned int dim;

    //! (x,y) coordinate representing the bottom-left corner of the square
    int corner[2];

    //! Maximum resolution, 2cm
    static const int maxRes = 1;
};
}
#endif /* _NODE_H */
