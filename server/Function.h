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

// Linking Gnash statically or dynamically with other modules is making a
// combined work based on Gnash. Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Gnash give you
// permission to combine Gnash with free software programs or libraries
// that are released under the GNU LGPL and with code included in any
// release of Talkback distributed by the Mozilla Foundation. You may
// copy and distribute such a system following the terms of the GNU GPL
// for all but the LGPL-covered parts and Talkback, and following the
// LGPL for the LGPL-covered parts.
//
// Note that people who make modified versions of Gnash are not obligated
// to grant this special exception for their modified versions; it is their
// choice whether to do so. The GNU General Public License gives permission
// to release a modified version without this exception; this exception
// also makes it possible to release a modified version which carries
// forward this exception.
// 
//
//

#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "impl.h"
#include "log.h"

namespace gnash {

/// ActionScript Function, either builtin or SWF-defined
class as_function : public as_object
{
public:

	virtual ~as_function() {}

	/// Dispatch.
	virtual void operator()(const fn_call& fn)=0;

	/// Get this function's "prototype" member (exported interface).
	//
	/// This is never NULL, and created on purpose if not provided
	/// at construction time. 
	as_object* getPrototype();


	/// Return true if this is a built-in class.
	virtual bool isBuiltin()  { return false; }

protected:

	/// Construct a function with given interface
	//
	/// If the given interface is NULL a default one
	/// will be provided, with constructor set as 'this'.
	as_function(as_object* iface);

	/// The "prototype" member.
	//
	/// Used for class constructor and members
	/// to be inherited by instances of this
	/// "Function" (class)
	///
	as_object*	_properties;
};

/// Any built-in function/class should be of this type
class builtin_function : public as_function
{

public:

	/// If 'func' parameter is NULL the function is not
	builtin_function(as_c_function_ptr func, as_object* iface)
		:
		as_function(iface),
		_func(func)
	{
	}

	/// Dispatch.
	virtual void operator()(const fn_call& fn)
	{
		assert(_func);
		_func(fn);
	}

	bool isBuiltin()  { return true; }

private:

	as_c_function_ptr _func;
};

/// SWF-defined Function 
class swf_function : public as_function
{

private:

	/// Action buffer containing the function definition
	const action_buffer* m_action_buffer;

	/// @@ might need some kind of ref count here, but beware cycles
	as_environment*	m_env;

	/// initial with-stack on function entry.
	std::vector<with_stack_entry>	m_with_stack;

	/// \brief
	/// Offset within the action_buffer where
	/// start of the function is found.
	size_t	m_start_pc;

	/// Length of the function within the action_buffer
	//
	/// This is currently expressed in bytes as the
	/// action_buffer is just a blog of memory corresponding
	/// to a DoAction block
	size_t m_length;

	struct arg_spec
	{
		int	m_register;
		tu_string	m_name;
	};
	std::vector<arg_spec>	m_args;
	bool	m_is_function2;
	uint8	m_local_register_count;

	/// used by function2 to control implicit
	/// arg register assignments
	uint16	m_function2_flags;

public:

	~swf_function();

	/// Default constructor
	//
	/// Creates a Function object inheriting
	/// the Function interface (apply,call)
	///
	//swf_function();

	/// Construct a Built-in ActionScript class 
	//
	/// The provided export_iface as_object is what will end
	/// up being the class's 'prototype' member, caller must
	/// make sure to provide it with a 'constructor' member
	/// pointing to the function that creates an instance of
	/// that class.
	/// All built-in classes derive from the Function
	/// built-in class, whose exported interface will be 
	/// accessible trought their __proto__ member.
	///
	/// @param export_iface the exported interface
	///
	//swf_function(as_object* export_iface);
	// USE THE builtin_function instead!

	/// \brief
	/// Create an ActionScript function as defined in an
	/// action_buffer starting at offset 'start'
	//
	/// NULL environment is allowed -- if so, then
	/// functions will be executed in the caller's
	/// environment, rather than the environment where they
	/// were defined.
	///
	swf_function(const action_buffer* ab,
		as_environment* env,
		size_t start,
		const std::vector<with_stack_entry>& with_stack);

	void	set_is_function2() { m_is_function2 = true; }

	void	set_local_register_count(uint8 ct) { assert(m_is_function2); m_local_register_count = ct; }

	void	set_function2_flags(uint16 flags) { assert(m_is_function2); m_function2_flags = flags; }

	void	add_arg(int arg_register, const char* name)
	{
		assert(arg_register == 0 || m_is_function2 == true);
		m_args.resize(m_args.size() + 1);
		m_args.back().m_register = arg_register;
		m_args.back().m_name = name;
	}

	void	set_length(int len)
	{
		assert(m_action_buffer);
		assert(len >= 0);
		assert(m_start_pc+len <= m_action_buffer->size());
		m_length = len;
	}

	/// Dispatch.
	void	operator()(const fn_call& fn);

	//void	lazy_create_properties();
};


void function_apply(const fn_call& fn);
void function_call(const fn_call& fn);

/// Initialize the global Function constructor
void function_init(as_object* global);

} // end of gnash namespace

// __FUNCTION_H__
#endif

