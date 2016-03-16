#include "DataLayerSet.h"

#include "DataLayer.h"
#include "InputData.h"
#include "DataLayerProcessor.h"
#include "Convertor.h"


Types::DataLayerSetPointer DataLayerSet::mThis = NULL;

Types::DataLayerSetPointer DataLayerSet::Get( ) {
    if( mThis == NULL ) {
        mThis = new DataLayerSet( );
    }
    return mThis;
}

DataLayerSet::~DataLayerSet( ) {

    for( Types::DataLayerMap::iterator iter = mDataLayerMap.begin( ); iter != mDataLayerMap.end( ); ++iter ) {
        delete iter->second;
    }

    if( mThis != NULL ) {
        delete mThis;
    }
}

DataLayerSet::DataLayerSet( ) {

}

void DataLayerSet::SetDataLayers( const Types::InputDataPointer data ) {
    Types::DataLayerProcessorPointer dataLayerProcessor = new DataLayerProcessor( );
    mDataLayerMap = dataLayerProcessor->ConvertReadDataIntoLayers( data );
    delete dataLayerProcessor;
}

Types::VariablePointer DataLayerSet::GetDefaultVariableFor( const std::string& name ) {

    Types::VariablePointer variable = NULL;

    Types::DataLayerMap::iterator iter = mDataLayerMap.find( name );
    
    if( iter != mDataLayerMap.end( ) ) {
        variable = iter->second->GetDefaultVariable( );
    }

    return variable;
}

Types::DataLayerPointer DataLayerSet::GetDataLayerWithName( const std::string& name ) {

    Types::DataLayerPointer dataLayer = NULL;

    Types::DataLayerMap::iterator iter = mDataLayerMap.find( name );

    if( iter != mDataLayerMap.end( ) ) {
        dataLayer = iter->second;
    } else {
        Logger::Get( )->LogMessage( "ERROR> DataLayer with name \"" + name + "\" was not found." );
    }

    return dataLayer;
}

float DataLayerSet::GetDataAtGeoCoordFor( const std::string name, const Types::GeoCoordPointer coord ) {

    float value = Constants::cMissingValue;

    Types::DataLayerPointer dataLayer = GetDataLayerWithName( name );

    if( dataLayer != NULL ) {
        value = dataLayer->GetDataAtGeoCoord( coord );
    }

    return value;
}

float DataLayerSet::GetDataAtIndicesFor( const std::string name, Types::IndicesPointer indices ) {

    float value = Constants::cMissingValue;

    Types::DataLayerPointer dataLayer = GetDataLayerWithName( name );

    if( dataLayer != NULL ) {
        value = dataLayer->GetDataAtIndices( indices );
    }

    return value;
}