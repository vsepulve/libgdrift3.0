#ifndef _ANALIZER_H_
#define _ANALIZER_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include <thread>
#include <mutex>

#include "WorkManager.h"
#include "ResultsReader.h"

using namespace std;

class Analizer{

protected:

private:
	WorkManager *manager;
	string file_base;
	string target_base;
	// Numero de threads de procesamiento
	unsigned int n_threads;

public:

	Analizer();
	Analizer(WorkManager *_manager, unsigned int _n_threads, string _file_base, string _target_base);
	virtual ~Analizer();
	
	
	
	void execute(unsigned int sim_id);
	
	
	
	
};

#endif