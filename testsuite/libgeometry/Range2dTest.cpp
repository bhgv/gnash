// 
//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "check.h"
#include "Range2d.h"
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;
using namespace gnash;
using namespace gnash::geometry;

int
main(int /*argc*/, char** /*argv*/)
{

	//
	// Test NULL range construction
	//

	Range2d<int> nullIntRange1;
	check(   nullIntRange1.isNull() );
	check( ! nullIntRange1.isWorld() );
	check( ! nullIntRange1.isFinite() );

	Range2d<int> nullIntRange2(nullRange);
	check_equals( nullIntRange1, nullIntRange2 );
	check( nullIntRange1 == nullIntRange2 );

	//
	// Test WORLD range construction
	//

	Range2d<int> worldIntRange1(worldRange);
	check( ! worldIntRange1.isNull() );
	check(   worldIntRange1.isWorld() );
	check( ! worldIntRange1.isFinite() );
	check( nullIntRange1 != worldIntRange1 );

	//
	// Test FINITE range construction
	//

	Range2d<int> fIntRange1(0, 0, 10, 10);

	check( ! fIntRange1.isNull() );
	check( ! fIntRange1.isWorld() );
	check(   fIntRange1.isFinite() );
	check( fIntRange1 != worldIntRange1 );
	check( fIntRange1 != nullIntRange1 );

	//
	// Test range Union 
	//

	Range2d<int> uIntNW1 = Union(nullIntRange1, worldIntRange1);
	Range2d<int> uIntNW2 = Union(worldIntRange1, nullIntRange1);

	// union is transitive
	check_equals( uIntNW1, uIntNW2 );

	// union of anything with world is world
	check_equals( uIntNW1, worldIntRange1 );

	Range2d<int> uIntNF1 = Union(nullIntRange1, fIntRange1);
	Range2d<int> uIntNF2 = Union(fIntRange1, nullIntRange1);

	// union is transitive
	check_equals( uIntNF1, uIntNF2 );

	// union of anything with null is no-op
	check_equals( uIntNF1, fIntRange1 );

	Range2d<int> uIntWF1 = Union(worldIntRange1, fIntRange1);
	Range2d<int> uIntWF2 = Union(fIntRange1, worldIntRange1);

	// union is transitive
	check_equals( uIntWF1, uIntWF2 );

	// union of anything with world is world
	check_equals( uIntWF1, worldIntRange1 );

	check_equals(
		Union( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(-10, -10, -5, -5)),
		Range2d<int>(-10, -10, 10, 10)
	);

	// disjoint
	check_equals(
		Union( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(-10, -10, 5, 5)),
		Range2d<int>(-10, -10, 10, 10)
	);

	// overlapping
	check_equals(
		Union( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(-10, -10, 5, 5)),
		Range2d<int>(-10, -10, 10, 10)
	);
	check_equals(
		Union( Range2d<float>(-0.2, -0.3, 0.7, 0.8),
			Range2d<float>(-0.1, -0.1, 0.8, 0.9)),
		Range2d<float>(-0.2, -0.3, 0.8, 0.9)
	);

	// inscribed
	check_equals(
		Union( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(2, 2, 5, 5)),
		Range2d<int>(0, 0, 10, 10)
	);
	check_equals(
		Union( Range2d<unsigned short>(2, 2, 8, 9),
			Range2d<unsigned short>(0, 1, 9, 10)),
		Range2d<unsigned short>(0, 1, 9, 10)
	);

	//
	// Test Intersection  / Intersects
	//

	Range2d<int> iIntNW1 = Intersection(nullIntRange1, worldIntRange1);
	Range2d<int> iIntNW2 = Intersection(worldIntRange1, nullIntRange1);

	// intersection is transitive
	check_equals( iIntNW1, iIntNW2 );

	// intersection of anything with null is null
	check_equals( iIntNW1, nullIntRange1 );
	check_equals( Intersect(nullIntRange1, worldIntRange1), false );

	// disjoint ranges
	check_equals(
		Intersection( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(-10, -10, -2, -3)),
		Range2d<int>() // NULL range !
	);
	check_equals(
		Intersection( Range2d<double>(-100.4, 50.5, -60.4, 60),
			Range2d<double>(-80, -10, -70, -9)),
		Range2d<double>() // NULL range !
	);
	check_equals(
		Intersect( Range2d<double>(-100.4, 50.5, -60.4, 60),
			Range2d<double>(-80, -10, -70, -9)),
		false
	);

	// overlapping ranges
	check_equals(
		Intersection( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(-10, -10, 5, 5) ),
		Range2d<int>(0, 0, 5, 5) 
	);
	check_equals(
		Intersect( Range2d<int>(0, 0, 10, 10),
			Range2d<int>(-10, -10, 5, 5)),
		true
	);

	// incribed ranges
	check_equals(
		Intersection( Range2d<unsigned short>(0, 0, 10, 10),
			Range2d<unsigned short>(2, 2, 5, 5)),
		Range2d<unsigned short>(2, 2, 5, 5) 
	);
	check_equals(
		Intersect( Range2d<unsigned short>(0, 0, 10, 10),
			Range2d<unsigned short>(2, 2, 5, 5)),
		true
	);
}

