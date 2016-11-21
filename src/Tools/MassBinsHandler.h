#ifndef MASSBINHANDLER
#define MASSBINHANDLER

#include "Types.h"
#include "Constants.h"
#include "Logger.h"

/** \brief Handles the mass bins to be used in model outputs */
class MassBinsHandler {
public:

    /** \brief The number of mass bins to be used for outputs */
    int mNumMassBins = 50;
    /** \brief A vector containing the masses corresponding to the mass bins */
    Types::FloatVector mMassBins;

    /** \brief Sets up mass bins based on an input file
    @param massBinsFile The filename containing the mass bin information */
    void SetUpMassBins( std::string massBinsFile );
};
#endif
