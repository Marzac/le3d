/**
	\file system.h
	\brief LightEngine 3D: Native OS / HW system manager
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.7

	The MIT License (MIT)
	Copyright (c) 2015-2018 Frédéric Meslin

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef LE_SYSTEM_H
#define LE_SYSTEM_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
/**
	\enum LE_SYSTEM_STATUS
	\brief application current status
*/
typedef enum {
	LE_SYSTEM_UNKNOWN			= 0x0,		/**< The status is unknown */
	LE_SYSTEM_ALRIGHT			= 0x01,		/**< The application is running  */
	LE_SYSTEM_EXIT_QUIT			= 0x02,		/**< The application is shutting down */
	LE_SYSTEM_EXIT_ABORT		= 0x04,		/**< The application has been aborted */
}LE_SYSTEM_STATUS;

/*****************************************************************************/
/**
	\class LeSystem
	\brief Handle OS / HW configuration and servicing
*/
class LeSystem
{
public:
	LeSystem();
	~LeSystem();

	void initialize();
	void terminate();
	void update();

	int getStatus();
	bool running;
	
private:
	int status;
	friend void SignalHandler(int signal);
};

extern LeSystem sys;

#endif // LE_SYSTEM_H
