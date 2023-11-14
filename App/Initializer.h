/*
 * Initializer.h
 *
 *  Created on: 1 paź 2019
 *      Author: dom
 */
#ifndef INITIALIZER_H_
#define INITIALIZER_H_

#include <iostream>
#include <memory>
#include <list>
#include <mutex>
#include <thread>

using namespace std;

class Initializer
{
    private:
		static mutex synch;
		static Initializer *initializerInstance;
		string port;

		Initializer();
    public:

		static Initializer * getInstance();
		void init(int argc, char *params[]);
		void begin();
};



#endif /* INITIALIZER_H_ */
