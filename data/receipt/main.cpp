#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>

using namespace std;

#include <tinyxml.h>
//#include <tinystr.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <jsoncpp/json/json.h>

using namespace std;

class Ingredient
{
public:
    unsigned int id;
    string  name;
    string  formula;
    string  description;
    unsigned int freq;              // frequency of the ingredient

    Ingredient(){ freq = 0; };      // default constructor
};

class Meal
{
public:
    string id;              // meal id
    string name;            // meal name
    vector< string > reactantVec;   // input
    vector< string > productVec;    // output
    string reversible;      // reversible
    string area;            // country
    string category;        // category
};

class Area
{
public:
    string name;        // category name
    unsigned int id;    // category id
};

string replaceSpaceWithUnderbar( string str )
{
    for( string::iterator it = str.begin(); it != str.end(); ++it ) {
        if( *it == ' ' ) {
            *it = '_';
        }
    }

    return str;
}

bool findIngredient( vector< Ingredient > &meta, string name, int &index )
{
	for( unsigned int i = 0; i < meta.size(); i++ ){

		if( meta[i].name == name ){
			index = i;
            // cerr << " meta[i].name = " << meta[i].name << " ?= " << name << endl;
			return true;
		}
	}

	return false;
}

void saveXml( string sysName, vector< Ingredient > &meta,
              vector< Meal > &react, vector< Area > &sub )
{
	TiXmlDocument xmlDoc;

	TiXmlNode *metaboliteElement = xmlDoc.InsertEndChild( TiXmlElement( "metabolites" ) );
    TiXmlNode *reactionElement = xmlDoc.InsertEndChild( TiXmlElement( "reactions" ) );
    TiXmlNode *subsysElement = xmlDoc.InsertEndChild( TiXmlElement( "subsystems" ) );

    // searching metabolites in the subsystems
    map< string, int > subMetaboliteMap;
    for( unsigned int i = 0; i < react.size(); i++ ){

        // searching metabolites in the subsystem - reactant
        for( unsigned int j = 0; j < react[i].reactantVec.size(); j++ ){
            int index = -1;
            string name = react[i].reactantVec[j];

            //cerr << "name = " << name << endl;
            if( sysName == react[i].area ){
            //if( sysName == react[i].category ){

                //string buf; // have a buffer string
                //stringstream ss( name ); // insert the string into a stream

                // a vector to hold our words
                //vector< string > tokens;
                //while (ss >> buf) tokens.push_back( buf );

                // check info in the metabolite list
                findIngredient( meta, name, index );
				//findIngredient( meta, tokens[ tokens.size()-1 ], index );
                //cerr << "  name = " << name << " index = " << index << endl;
                //cerr << "sysName = " << sysName << endl;
                assert( index != -1 );
				meta[ index ].freq += 1;
                subMetaboliteMap.insert( pair< string, int >( name, index ) );
            }
        }
    }

    // printing the metabolites
    for( map< string, int >::iterator it = subMetaboliteMap.begin(); it != subMetaboliteMap.end(); ++it ){

        int index = it->second;
        TiXmlNode* newNode = new TiXmlElement( "metabolite" );

        newNode->InsertEndChild( TiXmlElement( "name" ) )->InsertEndChild( TiXmlText( meta[index].name ) );
        newNode->InsertEndChild( TiXmlElement( "formula" ) )->InsertEndChild( TiXmlText( meta[index].formula ) );
        newNode->InsertEndChild( TiXmlElement( "description" ) )->InsertEndChild( TiXmlText( meta[index].description ) );

        metaboliteElement->InsertEndChild( *newNode );
    }

    // printing the reactions
    for( unsigned int i = 0; i < react.size(); i++ ){

        if( sysName == react[i].area ){
        //if( sysName == react[i].category ){

            TiXmlNode* newNode = new TiXmlElement( "reaction" );

            newNode->InsertEndChild( TiXmlElement( "id" ) )->InsertEndChild( TiXmlText( react[i].id ) );
            newNode->InsertEndChild( TiXmlElement( "name" ) )->InsertEndChild( TiXmlText( react[i].name ) );
            for( unsigned int j = 0; j < react[i].reactantVec.size(); j++ ){
                newNode->InsertEndChild( TiXmlElement( "reactant" ) )->InsertEndChild( TiXmlText( react[i].reactantVec[j] ) );
            }
            newNode->InsertEndChild( TiXmlElement( "reversible" ) )->InsertEndChild( TiXmlText( react[i].reversible ) );
            newNode->InsertEndChild( TiXmlElement( "subsystem" ) )->InsertEndChild( TiXmlText( react[i].area ) );
            //newNode->InsertEndChild( TiXmlElement( "subsystem" ) )->InsertEndChild( TiXmlText( react[i].category ) );

            reactionElement->InsertEndChild( *newNode );
        }
    }

    // printing the subsystem
    for( unsigned int i = 0; i < sub.size(); i++ ){

        if( sub[i].name == sysName ){

            TiXmlNode* newNode = new TiXmlElement( "subsystem" );

            newNode->InsertEndChild( TiXmlElement( "id" ) )->InsertEndChild( TiXmlText( to_string( sub[i].id ) ) );
            newNode->InsertEndChild( TiXmlElement( "name" ) )->InsertEndChild( TiXmlText( sub[i].name ) );
            //newNode->InsertEndChild( TiXmlElement( "color" ) )->InsertEndChild( TiXmlText( itS->second.color ) );
            //newNode->InsertEndChild( TiXmlElement( "x" ) )->InsertEndChild( TiXmlText( itS->second.x ) );
            //newNode->InsertEndChild( TiXmlElement( "y" ) )->InsertEndChild( TiXmlText( itS->second.y ) );

            subsysElement->InsertEndChild( *newNode );
        }
    }

    string outputname = "xml/" + sysName + ".xml";
    xmlDoc.SaveFile( outputname.c_str() );
}


int main( int argc, char **argv )
{
    vector < Ingredient > ingredientVec;
    vector < Meal > mealVec;
    vector < Area > areaVec;

    // areas
    boost::property_tree::ptree ptA;
    boost::property_tree::read_json( "meal/areas.json", ptA );
    //boost::property_tree::read_json( "meal/categories.json", ptA );

    BOOST_FOREACH( boost::property_tree::ptree::value_type &v, ptA ){

        //string test = pt.get< string >( "ingredients" );//v.second;
        // cerr << "area = " << v.first << endl;
        unsigned int index = 0;
        BOOST_FOREACH( boost::property_tree::ptree::value_type &c, ptA.get_child( v.first+"." ) ){
            // cerr << "areas = " << c.second.data() << endl;
            Area a;
            a.id = index;
            a.name = replaceSpaceWithUnderbar( c.second.data() );
            areaVec.push_back( a );
            index++;
        }
        // cerr << endl;
   }

#ifdef DEBUG
   cerr << "area:" << endl;
   for( unsigned int i = 0; i < areaVec.size(); i++ ){
       cerr << areaVec[i].id << ", " << areaVec[i].name << endl;
   }
   cerr << endl;
#endif // DEBUG

   // ingredients
   boost::property_tree::ptree ptI;
   boost::property_tree::read_json( "meal/ingredients.json", ptI );

   BOOST_FOREACH( boost::property_tree::ptree::value_type &v, ptI ){

       //string test = pt.get< string >( "ingredients" );//v.second;
       //cerr << "ingredients = " << v.first << endl;
       unsigned int index = 0;
       BOOST_FOREACH( boost::property_tree::ptree::value_type &c, ptI.get_child( v.first+"." ) ){
           //cerr << "ingredients = " << c.second.data() << endl;
           Ingredient i;
           i.id = index;
           i.name = replaceSpaceWithUnderbar( c.second.data() );
           i.formula = "none";
           i.description = "none";
           ingredientVec.push_back( i );
           index++;
       }
       //cerr << endl;
   }

#ifdef DEBUG
   cerr << "ingredients:" << endl;
   for( unsigned int i = 0; i < ingredientVec.size(); i++ ){
       cerr << ingredientVec[i].id << ", " << ingredientVec[i].name << endl;
   }
   cerr << endl;
#endif // DEBUG

   // meals
   boost::property_tree::ptree ptM;
   boost::property_tree::read_json( "meal/meals.json", ptM );

   unsigned int index = 0;
   BOOST_FOREACH( boost::property_tree::ptree::value_type &v, ptM ){

       Meal m;
       m.name = replaceSpaceWithUnderbar( v.first );
       m.id = m.name;
       // cerr << "meal = " << v.first << endl;
       BOOST_FOREACH( boost::property_tree::ptree::value_type &c, ptM.get_child( v.first+".ingredients." ) ){
           //cerr << "ingredients = " << c.second.data() << endl;
           m.reactantVec.push_back( replaceSpaceWithUnderbar( c.second.data() ) );
       }
       BOOST_FOREACH( boost::property_tree::ptree::value_type &c, ptM.get_child( v.first ) ){
           if( c.first == "category" ){
               m.category = c.second.data();     // category
               // cerr << "category = " << m.category << endl;
           }
           if( c.first == "area" ){
              //cerr << "area = " << c.second.data() << endl;
              m.reversible = "true";        // reversible
              m.area = c.second.data();     // category
           }
       }
       //cerr << endl;
       mealVec.push_back( m );
       index++;
   }

#ifdef DEBUG
   cerr << "meals:" << endl;
   for( unsigned int i = 0; i < mealVec.size(); i++ ){

       cerr << mealVec[i].id << ", " << mealVec[i].name << ", " << mealVec[i].area << endl;
       for( unsigned int j = 0; j < mealVec[i].reactantVec.size(); j++ ){

            cerr << mealVec[i].reactantVec[j] << ", ";
       }
       cerr << endl;
   }
   cerr << endl;
#endif // DEBUG

    // export the file
   for( unsigned int i = 0; i < areaVec.size(); i++ ){

      saveXml( areaVec[i].name, ingredientVec, mealVec, areaVec );

       //cerr << "Writing content to " << "../xml/" + filename << endl;
   }

   // export the metabolite frequency
   ofstream        off( "xml/ingredient_frequency.txt" );

   if ( !off ) {
       cerr << "Cannot open the target file : " << "xml/ingredient_frequency.txt" << endl;
       assert( false );
   }

   // sort
   multimap< int, Ingredient > iMap;
   for( unsigned int i = 0; i < ingredientVec.size(); i++ ){
       iMap.insert( pair< int, Ingredient >( ingredientVec[ i ].freq, ingredientVec[ i ] ) );
   }

   unsigned int id = 0;
   for( multimap< int, Ingredient >::iterator it = iMap.begin(); it != iMap.end(); it++ ){
       if( id == iMap.size()-1 ){
           off << it->second.name << "\t" << it->first;
       }
       else{
           off << it->second.name << "\t" << it->first << endl;
       }
       id++;
   }

/*
   for( unsigned int i = 0; i < ingredientVec.size(); i++ ){
       if( i == ingredientVec.size()-1 ){
           off << ingredientVec[ i ].name << "\t" << ingredientVec[ i ].freq;
       }
       else{
           off << ingredientVec[ i ].name << "\t" << ingredientVec[ i ].freq << endl;
       }
   }
*/
   return 0;
}
