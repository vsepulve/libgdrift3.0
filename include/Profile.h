#ifndef _PROFILE_H_
#define _PROFILE_H_

#include <iostream>
#include <fstream>
#include <assert.h>

#include <nlohmann/json.hpp>

#include <vector>

#include "ProfileMarker.h"
#include "ParsingUtils.h"

using json = nlohmann::json;
using namespace std;

class Profile{

private:
	unsigned char ploidy;
	vector<ProfileMarker> markers;
	
public:

	Profile();
	Profile(json &individual);
	Profile(const Profile &original);
	Profile& operator=(const Profile& original);
	virtual Profile *clone();
	virtual ~Profile();
	
	unsigned int getPloidy() const;
	
	const vector<ProfileMarker> &getMarkers() const;
	
	unsigned int getNumMarkers() const;
	
	const ProfileMarker &getMarker(unsigned int pos) const;
	
	void setPloidy(unsigned int _ploidy);
	
	void addMarker(ProfileMarker &marker);
	
	// Metodo de debug
	void print(){
		cout << "Profile::print - ploidy: " << (unsigned int)ploidy << "\n";
		cout << "Profile::print - n_markers: " << markers.size() << "\n";
		for( ProfileMarker marker : markers ){
			marker.print();
		}
	}
	
	// Metodos de Serializacion
	
	unsigned int serializedSize(){
		// Por ahora solo ploidy, n_markes y cada marker
		// Es posible que se usen otros datos globales mas adelante
		unsigned int n_bytes = 1 + sizeof(int);
		for(ProfileMarker marker : markers){
			n_bytes += marker.serializedSize();
		}
		return n_bytes;
	}
	
	void serialize(char *buff){
	
		*buff = (char)ploidy;
		buff += 1;
		
		unsigned int n_markers = markers.size();
		memcpy(buff, (char*)&n_markers, sizeof(int));
		buff += sizeof(int);
		
		for(ProfileMarker marker : markers){
			marker.serialize(buff);
			buff += marker.serializedSize();
		}
		
	}
	
	unsigned int loadSerialized(char *buff){
//		cout << "Profile::loadSerialized - Inicio\n";
		
		char *buff_original = buff;
		
		ploidy = (unsigned char)(*buff);
		buff += 1;
		
		unsigned int n_markers = 0;
		memcpy((char*)&n_markers, buff, sizeof(int));
		buff += sizeof(int);
		
//		for(ProfileMarker marker : markers){
		for(unsigned int i = 0; i < n_markers; ++i){
			ProfileMarker marker;
			buff += marker.loadSerialized(buff);
			markers.push_back(marker);
		}
		
//		cout << "Profile::loadSerialized - Fin (bytes usados: " << (buff - buff_original) << ")\n";
		return (buff - buff_original);
	}
	
	
	
	
	
	
	
	
	
	
};

#endif
