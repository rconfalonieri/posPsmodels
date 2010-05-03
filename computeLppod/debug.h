/*
 * debug.h
 *
 *  Created on: 18/dic/2009
 *      Author: confalonieri
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define PRINT(x)
#else
#define PRINT(x) \
	cout << #x << ":\t" << x << endl;



#endif /* DEBUG_H_ */
