/*
 *  If_ListElement.C -- class definition
 *
 *  rf, 6/22/94
 */

/*
    Copyright (C) 1994--1998  Ruediger Franke

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library (file COPYING.LIB);
    if not, write to the Free Software Foundation, Inc.,
    59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "If_List.h"
#include "If_ListElement.h"

//-----------------------------------------------------------------------------
If_ListElement::If_ListElement()
{
  _prev = _next = NULL;
}

//-----------------------------------------------------------------------------
If_ListElement::~If_ListElement()
{
  if (_prev)
    _prev->_next = _next;
  if (_next)
    _next->_prev = _prev;
}


//=============================================================================
