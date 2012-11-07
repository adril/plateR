// Message.h

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <deque>
#include "VSP.hh"

#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#pragma once

using namespace System;

namespace Message1 {

	public ref class Class1
	{
		// TODO: Add your methods for this class here.
	public:
		Message();
		Message(Message const &);
		virtual ~Message();

		char*		getData() const;
		char*         getLogin() const;
		size_t	getLength() const;
		char		getType() const;
		size_t	getBodyLength() const;

		void		encodeHeader(char, const char *login);
		void		decodeHeader(void);

		virtual void	encodeBody();

	protected:
		size_t	bodyLength_;
		char		*data_;
		char		type_;
		char		*login_;
	public:
		std::deque<std::string> list;
	};
}

#endif /* !__MESSAGE_HPP__ */
