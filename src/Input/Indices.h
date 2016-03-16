#ifndef INDICES
#define	INDICES

class Indices {
public:
    Indices( const unsigned int&, const unsigned int&, const unsigned int& );
    Indices( const unsigned int&, const unsigned int& );
    ~Indices( );

    unsigned int GetX( ) const;
    unsigned int GetY( ) const;
    unsigned int GetZ( ) const;

    void SetX( const unsigned int& );
    void SetY( const unsigned int& );
    void SetZ( const unsigned int& );

private:
    unsigned int mX;
    unsigned int mY;
    unsigned int mZ;
};

#endif

