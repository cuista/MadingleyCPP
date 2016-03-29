#ifndef DATAINDICES
#define	DATAINDICES

#include "Constants.h"

class DataIndices {
public:
    DataIndices( const unsigned&, const unsigned&, const unsigned&, const Constants::eIndexDomainTypes );
    DataIndices( const unsigned&, const unsigned&, const Constants::eIndexDomainTypes );
    ~DataIndices( );

    unsigned GetDataX( ) const;
    unsigned GetDataY( ) const;

    unsigned GetUserX( ) const;
    unsigned GetUserY( ) const;

    unsigned GetZ( ) const;

    void SetDataX( const unsigned& );
    void SetDataY( const unsigned& );
    void SetDataZ( const unsigned& );

    void SetUserX( const unsigned& );
    void SetUserY( const unsigned& );

    void SetZ( const unsigned& );
private:
    void Initialise( const unsigned&, const unsigned&, const unsigned&, const Constants::eIndexDomainTypes );
    unsigned CalculateUserX( const unsigned& ) const;
    unsigned CalculateUserY( const unsigned& ) const;

    unsigned mDataX;
    unsigned mDataY;

    unsigned mUserX;
    unsigned mUserY;

    unsigned mZ;
};

#endif

