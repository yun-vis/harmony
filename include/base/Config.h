
#ifndef __BASE__CONFIG_H__
#define __BASE__CONFIG_H__

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace::std;

namespace Base {

	class Config {

		private:
			map< string, string > m_map;

		public:

			Config( const string& filename = "" ) {
				loadConfigFile( filename );
			}
			~Config(){}

			double getlf( const string& key ) const;
			float getf( const string& key ) const;
			int geti( const string& key ) const;
			string gets( const string& key ) const;
			bool has( const string& key ) const;

		private:

			void split( const string& in, vector< string >& out );
			void loadConfigFile( const string& filename );
		};

} // namespace Base


#endif // __BASE__CONFIG_H__
