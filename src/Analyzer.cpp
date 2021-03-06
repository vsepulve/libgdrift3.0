#include "Analyzer.h"

Analyzer::Analyzer(){
	manager = NULL;
	n_threads = 0;
	percentage = 0;
	file_base = "";
	target_base = "";
	results_base = "";
}

Analyzer::Analyzer(WorkManager *_manager, unsigned int _n_threads, float _percentage, string _file_base, string _target_base, string _results_base){
	manager = _manager;
	n_threads = _n_threads;
	percentage = _percentage;
	file_base = _file_base;
	target_base = _target_base;
	results_base = _results_base;
}

Analyzer::~Analyzer(){
	// notar que el manager es del daemon
	manager = NULL;
}

void Analyzer::execute(unsigned int sim_id){

	assert( manager != NULL);
	assert( n_threads > 0);
	
	unsigned int feedback = manager->getFeedback(sim_id);
	
	cout << "Analyzer::execute - Inicio (sim_id: " << sim_id << ", feedback: " << feedback << ")\n";
	
	// Leer los archivos de los threads de la simulacion
	string sim_file_base = file_base;
	sim_file_base += to_string(sim_id);
	sim_file_base += "_f";
	sim_file_base += std::to_string(feedback);
	sim_file_base += "_";
	
	unsigned int n_stats = manager->getNumStatistics(sim_id);
	unsigned int n_params = manager->getNumParams(sim_id);
	
	ResultsReader reader;
	reader.readThreadsData(sim_file_base, n_threads, n_stats, n_params);
	
	// Potencialmente agrupar los archivos en un solo archivo consolidado (equivalente al de generate_statistics)
	
	// Cargar el target (o pedirlo al manager, aun no esta decidido)
	string target_file = target_base;
	target_file += to_string(manager->getProjectId(sim_id));
	target_file += ".txt";
	reader.setTarget(target_file);
	
	// Si hay resultados previos, cargar la peor distancia para normalizar
	if( feedback > 0 ){
		// Ruta de archivo de resultados anterior
		string results_file = results_base;
		results_file += to_string(sim_id);
		results_file += "_f";
		results_file += std::to_string(feedback - 1);
		results_file += ".txt";
		
		fstream file_reader(results_file, fstream::in);
		if( file_reader.good() ){
			char buff[1024];
			file_reader.getline(buff, 1024);
			file_reader.close();
			string line(buff);
			stringstream toks(line);
			string tok;
			double min, max, cut;
			toks >> tok;
			toks >> min;
			toks >> max;
			toks >> cut;
			cout << "Analyzer::execute - Previous distance: " << tok << " | " << min << " | " << max << " | " << cut << "\n";
			if( max > 0.0 ){
				reader.addWorstDistance(max);
			}
		}
	}
	
	// Normaliza tanto los datos como el target
	reader.normalize();
	
	// Calcula la distancia de cada dato normalizado al target
	reader.computeDistances();
	
	// Ordena por distancia y selecciona los mejores resultados para el training
	reader.selectBestResults(percentage);
	
	// Retorna pares <mean, stddev> de cada parametro calculado de los datos seleccionados
	vector< pair<double, double> > dist_post = reader.getPosteriori();
	
	// Guardar resultados (para registro en caso de seguir entrenando, o como resultado final)
	// Notar que el servicio de query deberia usar los datos disponibles para calcular online
	// En ese caso, es posible que haya datos incompletos, simplemente los omito en ese caso
	// Tambien notar que para guardar los resultados seria util darle nombre a cada parametro
	// El metodo siguiente asume el mismo orden de lectura en el builder
	vector<string> params_names = manager->getParams(sim_id);
	manager->addFeedback(sim_id);
	
	assert( params_names.size() == dist_post.size() );
	
	string results_file = results_base;
	results_file += to_string(sim_id);
	results_file += "_f";
	results_file += std::to_string(feedback);
	results_file += ".txt";
	
	fstream writer(results_file, fstream::out | fstream::trunc);
	
	// En la primera linea pongo las distancias
	writer << "Distances\t";
	writer << reader.getMinDistance() << "\t";
	writer << reader.getMaxDistance() << "\t";
	writer << reader.getCutDistance() << "\t";
	writer << "\n";
	
	// En la primera linea pongo las distancias
	writer << "DistancesNorm\t";
	writer << (reader.getMinDistance() / reader.getMaxDistance()) << "\t";
	writer << (reader.getMaxDistance() / reader.getMaxDistance()) << "\t";
	writer << (reader.getCutDistance() / reader.getMaxDistance()) << "\t";
	writer << "\n";
	
	// Siguen las distribuciones
	for( unsigned int i = 0; i < params_names.size(); ++i ){
		writer << params_names[i] << "\t";
		writer << dist_post[i].first << "\t";
		writer << dist_post[i].second << "\t";
		writer << "\n";
	}
	writer.close();
	
	// Esta primera version, simplemente realiza una unica pasada de feedback, asi de simple
	// El batch size se determina al principio (como parametro, por el servicio de inicio, o fijo por el demonio)
	// De este modo, siempre se realizaran 2 * batch_size simulaciones
	if( feedback == 0 ){
		// Actualizar distribuciones y agregar nuevas simulaciones
		cout << "Analyzer::execute - feedback == 0, agregando nuevo batch\n";
		manager->addTraining(sim_id, dist_post);
	}
	else{
		// los resultados ya estan guardados, bastaria con terminar aqui
		cout << "Analyzer::execute - feedback > 0, terminando simulation " << sim_id <<" \n";
		
	}
	
	
}


void Analyzer::partialResults(unsigned int sim_id){

	assert( manager != NULL);
	assert( n_threads > 0);
	
	unsigned int feedback = manager->getFeedback(sim_id);
	unsigned int finished = manager->getFinished(sim_id);
	unsigned int total = manager->getTotal(sim_id);
	
	cout << "Analyzer::partialResults - Inicio (sim_id: " << sim_id << ", feedback: " << feedback << ", finished: " << finished << " / " << total << ")\n";
	
	
	
	
	
}









