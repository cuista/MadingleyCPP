#ifndef FUNCTIONALGROUPDEFINITIONS_H
#define FUNCTIONALGROUPDEFINITIONS_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include "Constants.h"
using namespace std;
/** \file FunctionalGroupDefinitions.h
 * \brief the FunctionalGroupDefinitions header file
 */

/** \brief Reads in and performs look-ups on functional group definitions
    @remark Mass bins values currently defined as middle of each mass bins</remarks>
 */
class FunctionalGroupDefinitions {
public:
    //----------------------------------------------------------------------------------------------
    //Variables
    //----------------------------------------------------------------------------------------------
    /** \brief A lookup device: sorted dictionary keyed by Functional Trait and valued by a sorted dictionary itself keyed by Unique Functional Trait Values and valued by an integer array of functional group indices corresponding to each functional trait value
     */
    map<string, map<string, vector<int>>> IndexLookupFromTrait;
    /** \brief A sorted list of all of the properties of functional groups and their values */
    map<string, vector<double> > FunctionalGroupProperties;
    /** \brief Dictionary to allow traits of functional groups to be looked up based on the functional group index*/
    map<string, vector<string> > TraitLookupFromIndex;
    /** \brief A list of the indices of all functional groups in the model*/
    vector<int> AllFunctionalGroupsIndex;
    //----------------------------------------------------------------------------------------------
    //Methods
    //----------------------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------------------
    //Empty contructor to get compilation going

    FunctionalGroupDefinitions( ) {
        ;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Constructor for the functional group definitions: reads in the specified functional group definition file, 
    Constructs lookup tables, mass ranges and initial cohort numbers in each functional group
    @param fileName The name of the functional group definition file to be read in
    @param outputPath The path to the output folder, in which to copy the functional group definitions file
     */
    FunctionalGroupDefinitions( string fileName ) {
        cout << "Reading those functional group definitions" << endl;
        fileName = Constants::cConfigurationDirectory + fileName;
        ifstream infile( fileName.c_str( ) );
        if( infile.is_open( ) ) {

            string l;
            vector<string>header, category;
            getline( infile, l );
            //trim off newline character
            l.pop_back( );
            istringstream s( l );
            //split out the comma-separated and underscore separated header
            while( s.good( ) ) {
                string tmp;
                getline( s, tmp, ',' );
                transform( tmp.begin( ), tmp.end( ), tmp.begin( ), ::tolower );
                //split out the header category (definition.property or note)
                istringstream splt( tmp );
                string dp, op;
                getline( splt, dp, '_' );
                category.push_back( dp );
                getline( splt, op, '_' );
                header.push_back( op );
            }
            int count = 0;
            //retrieve the lines defining each functional group

            while( infile.good( ) ) {

                string l, data;
                getline( infile, l );
                if( infile.good( ) ) {
                    l.pop_back( );
                    AllFunctionalGroupsIndex.push_back( count );

                    if( l.length( ) > 1 ) {
                        istringstream s( l );
                        //step through the columns for this functional group
                        for( unsigned i = 0; i < header.size( ); i++ ) {
                            getline( s, data, ',' );
                            transform( data.begin( ), data.end( ), data.begin( ), ::tolower );

                            if( category[i] == "definition" ) {
                                //for each trait, store the value for a given functional group
                                //indexed by functional group number
                                TraitLookupFromIndex[header[i]].push_back( data );
                                //for a given trait, store the functional group number
                                //which has a given value for that trait
                                IndexLookupFromTrait[header[i]][data].push_back( count );
                            }
                            //Otherwise get the value for the given property
                            //for this functional group
                            if( category[i] == "property" ) {
                                FunctionalGroupProperties[header[i]].push_back( atof( data.c_str( ) ) );
                            }
                        }
                    }
                    count++;
                }
            }


        } else {
            cout << "Something wrong with functional group definitions file " << fileName << endl;
        }
        infile.close( );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Return the value of a biological parameter for a given parameter and functional group
    @param propertyName The name of the biological parameter
    @param functionalGroup Functional group index
    @return The value of the biological parameter for the specified functional group */

    double GetBiologicalPropertyOneFunctionalGroup( string propertyName, int functionalGroup ) {
        transform( propertyName.begin( ), propertyName.end( ), propertyName.begin( ), ::tolower );
        return FunctionalGroupProperties[propertyName][functionalGroup];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get values of a functional group property for all functional groups
    @param propertyName The name of the property to get values for
    @return The values of a functional group property for all functional groups */
    vector<double> GetBiologicalPropertyAllFunctionalGroups( string propertyName ) {
        transform( propertyName.begin( ), propertyName.end( ), propertyName.begin( ), ::tolower );
        return FunctionalGroupProperties[propertyName];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Retrieves the values for all traits defined in the model
    @return String array of traits defined for the model
     */
    vector<string> GetTraits( ) {
        vector<string> Traits;

        for( auto var: TraitLookupFromIndex ) {
            Traits.push_back( var.first );
        }

        return Traits;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Retrieves the trait values for all traits defined in the model
    @param Trait The trait for which trait values are to be found
    @returns String array of trait values for the specifiec trait
     */
    vector<string> GetUniqueTraitValues( string Trait ) {
        vector<string> TraitValues;
        transform( Trait.begin( ), Trait.end( ), Trait.begin( ), ::tolower );

        map<string, vector<int> > temp = IndexLookupFromTrait[Trait];

        for( auto var: temp ) {
            TraitValues.push_back( var.first );
        }

        return TraitValues;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Returns a string of Trait Names associated with the specified search trait and functional group index value

    @param searchTrait The name of the trait to get values for
    @param functionalGroupIndex The functional group index to return the trait value for
    @return The value of the specified trait for the specified functional group*/

    string GetTraitNames( string searchTrait, int functionalGroupIndex ) {
        transform( searchTrait.begin( ), searchTrait.end( ), searchTrait.begin( ), ::tolower );

        return TraitLookupFromIndex[searchTrait][functionalGroupIndex];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the values of a set of specified traits for a specified functional group
    @param searchTraits A vector of trait names to get values for
    @param functionalGroupIndex The functional group index to return trait values for
    @returns A vector of values of the specified traits for a specified functional group*/

    vector<string> GetTraitNames( vector<string> searchTraits, int functionalGroupIndex ) {
        vector<string> TraitNames( searchTraits.size( ) );
        //
        for( auto sT: searchTraits ) {
            transform( sT.begin( ), sT.end( ), sT.begin( ), ::tolower );
            TraitNames.push_back( TraitLookupFromIndex[sT][functionalGroupIndex] );
        }
        return TraitNames;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the functional group indices that have specified values of specified traits
    @param searchTraits Vector of trait names to search for
    @param searchTraitValues Vector of trait values to search for
    @param intersection Whether the intersection of the indices for the traits should be returned, otherwise return the union of the indices
    @return A vector of functional group indices with the specified values of the specified traits
     */
    vector<int> GetFunctionalGroupIndex( vector<string> searchTraits, vector<string> searchTraitValues, bool intersection ) {
        // Check that the numbers of traits and of trait values specified are equal
        vector<int> Result;
        assert( ( searchTraits.size( ) == searchTraitValues.size( ) ) && "Unequal search string arrays" );
        for( auto sT: searchTraits ) {
            if( IndexLookupFromTrait.count( sT ) != 0 ) {
                for( auto V: searchTraitValues )
                    if( IndexLookupFromTrait[sT].count( V ) != 0 ) {
                        copy( IndexLookupFromTrait[sT][V].begin( ), IndexLookupFromTrait[sT][V].end( ), Result.end( ) );
                    }
            }
        }
        sort( Result.begin( ), Result.end( ) );
        if( intersection )unique( Result.begin( ), Result.end( ) );
        return Result;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Function to return the integer index values for functional groups corresponding to given trait and trait value pair combinations.
    Overloaded to accept a single string rather than an array in the traits to search and the trait values - both must be single strings
    @param searchTraits String of Trait names to search for trait values within
    @param searchTraitValues String of string Trait Values to find functional group indices for
    @param intersection Boolean statement indicating if you want the intersection of the indices. Only valid if more than one Trait and Trait Value pair.
    //        /// True means give intersection. False means give the union of indices
    @return Int array containing functional group indices corresponding to the given search conditions
     */
    vector<int> GetFunctionalGroupIndex( string searchTraits, string searchTraitValues, bool intersection ) {

        //List to hold the index vectors for each trait trait value pair
        vector<int> IndexList;
        transform( searchTraits.begin( ), searchTraits.end( ), searchTraits.begin( ), ::tolower );
        transform( searchTraitValues.begin( ), searchTraitValues.end( ), searchTraitValues.begin( ), ::tolower );
        //Sorted dictionary to hold the trait value index list sorted dictionary from the lookup table
        map<string, vector<int>> TraitIndexList;

        //Check if the trait name is in the lookup table and if so pull out the <trait value, index vector> sorted dictionary for it
        if( IndexLookupFromTrait.count( searchTraits ) != 0 ) {
            //Check if the trait value string is found in the lookup table and if found pull out the index vector for it
            //and add it to the List of these for processing - intersection of union
            if( IndexLookupFromTrait[searchTraits].count( searchTraitValues ) != 0 ) {
                return IndexLookupFromTrait[searchTraits][searchTraitValues];
            }//If trait value string not found then show error message
            else {
                cout << searchTraitValues << endl;
                cout << "Trait Value to search for not found in lookup tables" << endl;
                exit( 1 );
            }
        }//If trait name string not found then show error message
        else {
            cout << "Trait to search for not found in lookup tables" << endl;
            exit( 1 );
        }
        sort( IndexList.begin( ), IndexList.end( ) );
        if( intersection )unique( IndexList.begin( ), IndexList.end( ) );
        return IndexList;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Returns number of functional groups 
       @returns>Number of functional groups*/
    int GetNumberOfFunctionalGroups( ) {
        return (AllFunctionalGroupsIndex.size( ) );
    }
};
#endif