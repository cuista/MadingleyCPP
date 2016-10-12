#ifndef FUNCTIONALGROUPDEFINITIONS_H
#define FUNCTIONALGROUPDEFINITIONS_H

#include "Constants.h"
#include "Logger.h"
#include "Types.h"
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
    Types::Integer2DVectorMap mIndexLookupFromTrait;
    /** \brief A sorted list of all of the properties of functional groups and their values */
    Types::DoubleVectorMap mFunctionalGroupProperties;
    /** \brief Dictionary to allow traits of functional groups to be looked up based on the functional group index*/
    Types::StringVectorMap mTraitLookupFromIndex;
    /** \brief A list of the indices of all functional groups in the model*/
    Types::IntegerVector mAllFunctinoalGroupsIndex;
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
    FunctionalGroupDefinitions( std::string fileName ) {
        Logger::Get( )->LogMessage( "Reading \"" + fileName + "\" functional group definitions." );
        fileName = Constants::cConfigurationDirectory + fileName;
        std::ifstream infile( fileName.c_str( ) );
        if( infile.is_open( ) ) {

            std::string l;
            Types::StringVector header, category;
            getline( infile, l );
            //trim off newline character
            l.pop_back( );
            std::istringstream s( l );
            //split out the comma-separated and underscore separated header
            while( s.good( ) ) {
                std::string tmp;
                getline( s, tmp, ',' );
                std::transform( tmp.begin( ), tmp.end( ), tmp.begin( ), ::tolower );
                //split out the header category (definition.property or note)
                std::istringstream splt( tmp );
                std::string dp, op;
                getline( splt, dp, '_' );
                category.push_back( dp );
                getline( splt, op, '_' );
                header.push_back( op );
            }
            int count = 0;
            //retrieve the lines defining each functional group

            while( infile.good( ) ) {

                std::string l, data;
                getline( infile, l );
                if( infile.good( ) ) {
                    l.pop_back( );
                    mAllFunctinoalGroupsIndex.push_back( count );

                    if( l.length( ) > 1 ) {
                        std::istringstream s( l );
                        //step through the columns for this functional group
                        for( unsigned i = 0; i < header.size( ); i++ ) {
                            getline( s, data, ',' );
                            std::transform( data.begin( ), data.end( ), data.begin( ), ::tolower );

                            if( category[i] == "definition" ) {
                                //for each trait, store the value for a given functional group
                                //indexed by functional group number
                                mTraitLookupFromIndex[ header[ i ] ].push_back( data );
                                //for a given trait, store the functional group number
                                //which has a given value for that trait
                                mIndexLookupFromTrait[ header[ i ] ][ data ].push_back( count );
                            }
                            //Otherwise get the value for the given property
                            //for this functional group
                            if( category[i] == "property" ) {
                                mFunctionalGroupProperties[ header[ i ] ].push_back( atof( data.c_str( ) ) );
                            }
                        }
                    }
                    count++;
                }
            }


        } else {
            Logger::Get( )->LogMessage( "Something wrong with functional group definitions file " + fileName );
        }
        infile.close( );

    }
    //----------------------------------------------------------------------------------------------

    /** \brief Return the value of a biological parameter for a given parameter and functional group
    @param propertyName The name of the biological parameter
    @param functionalGroup Functional group index
    @return The value of the biological parameter for the specified functional group */

    double GetBiologicalPropertyOneFunctionalGroup( std::string propertyName, int functionalGroup ) {
        std::transform( propertyName.begin( ), propertyName.end( ), propertyName.begin( ), ::tolower );
        return mFunctionalGroupProperties[ propertyName ][ functionalGroup ];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get values of a functional group property for all functional groups
    @param propertyName The name of the property to get values for
    @return The values of a functional group property for all functional groups */
    Types::DoubleVector GetBiologicalPropertyAllFunctionalGroups( std::string propertyName ) {
        std::transform( propertyName.begin( ), propertyName.end( ), propertyName.begin( ), ::tolower );
        return mFunctionalGroupProperties[ propertyName ];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief  Retrieves the values for all traits defined in the model
    @return String array of traits defined for the model
     */
    Types::StringVector GetTraits( ) {
        Types::StringVector traits;

        for( auto var : mTraitLookupFromIndex ) {
            traits.push_back( var.first );
        }

        return traits;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief    Retrieves the trait values for all traits defined in the model
    @param Trait The trait for which trait values are to be found
    @returns String array of trait values for the specifiec trait
     */
    Types::StringVector GetUniqueTraitValues( std::string trait ) {
        Types::StringVector traitValues;
        std::transform( trait.begin( ), trait.end( ), trait.begin( ), ::tolower );

        Types::IntegerVectorMap temp = mIndexLookupFromTrait[ trait ];

        for( auto var : temp ) {
            traitValues.push_back( var.first );
        }

        return traitValues;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Returns a string of Trait Names associated with the specified search trait and functional group index value

    @param searchTrait The name of the trait to get values for
    @param functionalGroupIndex The functional group index to return the trait value for
    @return The value of the specified trait for the specified functional group*/

    std::string GetTraitNames( std::string searchTrait, int functionalGroupIndex ) {
        std::transform( searchTrait.begin( ), searchTrait.end( ), searchTrait.begin( ), ::tolower );

        return mTraitLookupFromIndex[ searchTrait ][ functionalGroupIndex ];
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the values of a set of specified traits for a specified functional group
    @param searchTraits A vector of trait names to get values for
    @param functionalGroupIndex The functional group index to return trait values for
    @returns A vector of values of the specified traits for a specified functional group*/

    Types::StringVector GetTraitNames( Types::StringVector searchTraits, int functionalGroupIndex ) {
        Types::StringVector traitNames( searchTraits.size( ) );
        //
        for( auto sT : searchTraits ) {
            std::transform( sT.begin( ), sT.end( ), sT.begin( ), ::tolower );
            traitNames.push_back( mTraitLookupFromIndex[ sT ][ functionalGroupIndex ] );
        }
        return traitNames;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Get the functional group indices that have specified values of specified traits
    @param searchTraits Vector of trait names to search for
    @param searchTraitValues Vector of trait values to search for
    @param intersection Whether the intersection of the indices for the traits should be returned, otherwise return the union of the indices
    @return A vector of functional group indices with the specified values of the specified traits
     */
    Types::IntegerVector GetFunctionalGroupIndex( Types::StringVector searchTraits, Types::StringVector searchTraitValues, bool intersection ) {
        // Check that the numbers of traits and of trait values specified are equal
        Types::IntegerVector result;
        assert( ( searchTraits.size( ) == searchTraitValues.size( ) ) && "Unequal search string arrays" );
        for( auto sT : searchTraits ) {
            if( mIndexLookupFromTrait.count( sT ) != 0 ) {
                for( auto V : searchTraitValues )
                    if( mIndexLookupFromTrait[ sT ].count( V ) != 0 ) {
                        std::copy( mIndexLookupFromTrait[ sT ][ V ].begin( ), mIndexLookupFromTrait[ sT ][ V ].end( ), result.end( ) );
                    }
            }
        }
        std::sort( result.begin( ), result.end( ) );
        if( intersection ) std::unique( result.begin( ), result.end( ) );
        return result;
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
    Types::IntegerVector GetFunctionalGroupIndex( std::string searchTraits, std::string searchTraitValues, bool intersection ) {

        //List to hold the index vectors for each trait trait value pair
        Types::IntegerVector indexList;
        std::transform( searchTraits.begin( ), searchTraits.end( ), searchTraits.begin( ), ::tolower );
        std::transform( searchTraitValues.begin( ), searchTraitValues.end( ), searchTraitValues.begin( ), ::tolower );
        //Sorted dictionary to hold the trait value index list sorted dictionary from the lookup table
        //Types::IntegerVectorMap traitIndexList;

        //Check if the trait name is in the lookup table and if so pull out the <trait value, index vector> sorted dictionary for it
        if( mIndexLookupFromTrait.count( searchTraits ) != 0 ) {
            //Check if the trait value string is found in the lookup table and if found pull out the index vector for it
            //and add it to the List of these for processing - intersection of union
            if( mIndexLookupFromTrait[ searchTraits ].count( searchTraitValues ) != 0 ) {
                return mIndexLookupFromTrait[ searchTraits ][ searchTraitValues ];
            }//If trait value string not found then show error message
            else {
                Logger::Get( )->LogMessage( searchTraitValues );
                Logger::Get( )->LogMessage( "Trait Value to search for not found in lookup tables" );
                exit( 1 );
            }
        }//If trait name string not found then show error message
        else {
            Logger::Get( )->LogMessage( "Trait to search for not found in lookup tables" );
            exit( 1 );
        }
        std::sort( indexList.begin( ), indexList.end( ) );
        if( intersection )std::unique( indexList.begin( ), indexList.end( ) );
        return indexList;
    }
    //----------------------------------------------------------------------------------------------

    /** \brief Returns number of functional groups 
       @returns>Number of functional groups*/
    int GetNumberOfFunctionalGroups( ) {
        return mAllFunctinoalGroupsIndex.size( );
    }
};
#endif