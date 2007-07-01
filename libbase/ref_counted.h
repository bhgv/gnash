// 
//   Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

/* $Id: ref_counted.h,v 1.11 2007/07/01 17:34:59 strk Exp $ */

#ifndef GNASH_REF_COUNTED_H
#define GNASH_REF_COUNTED_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tu_config.h" // for DSOEXPORT (better move that define in some other file?)

#include <cassert>
#include <boost/detail/atomic_count.hpp>

namespace gnash {

/// \brief
/// For stuff that's tricky to keep track of w/r/t ownership & cleanup.
/// The only use for this class seems to be for putting derived
/// classes in smart_ptr
///
class DSOEXPORT ref_counted
{

private:

	typedef long Counter;
	// boost atomic_counter is thread-safe, but doesn't
	// provide a copy constructor.
	//typedef mutable boost::detail::atomic_count Counter;

	mutable Counter m_ref_count;
	
protected:

	// A ref-counted object only deletes self,
	// must never be explicitly deleted !
	virtual ~ref_counted()
	{
		assert(m_ref_count == 0);
	}

public:
	ref_counted()
		:
		m_ref_count(0)
	{
	}

	void	add_ref() const
	{
		assert(m_ref_count >= 0);
		++m_ref_count;
	}

	void	drop_ref() const
	{
		assert(m_ref_count > 0);
		if (!--m_ref_count)
		{
			// Delete me!
			delete this;
		}
	}

	long	get_ref_count() const { return m_ref_count; }

	// These two methods are defined as a temporary hack to 
	// easy transition to the GC model. Was added when introducing
	// mixed ref-counted AND gc-collected classes to avoid touching
	// all ref-counted classes when GNASH_USE_GC is defined.
	// If this design convinces us we'll be removing these two
	// methods and all the calls from ref-counted classes.
	//
	void setReachable() { assert(m_ref_count > 0); }
	bool isReachable() const { return true; }
};

} // namespace gnash

#endif // GNASH_REF_COUNTED_H
