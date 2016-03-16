#ifndef GEOCOORD
#define	GEOCOORD

class GeoCoord {
public:
    GeoCoord( const float&, const float&, const float& );
    GeoCoord( const float&, const float& );
    ~GeoCoord( );

    float GetLongitude( ) const;
    float GetLatitude( ) const;
    float GetDepth( ) const;

    void SetLongitude( const float& );
    void SetLatitude( const float& );
    void SetDepth( const float& );

private:
    float mLongitude;
    float mLatitude;
    float mDepth;
};

#endif
