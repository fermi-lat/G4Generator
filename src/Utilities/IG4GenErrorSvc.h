#ifndef IG4GenErrorSvc_H
#define IG4GenErrorSvc_H 1

#include "GaudiKernel/IInterface.h"
#include <string>

static const InterfaceID IID_IG4GenErrorSvc("IG4GenErrorSvc",1,0) ;

/**   
* @class IG4GenErrorSvc
*
* Interface to data and features shared by all CalRecon actors.
*
* $Header: /nfs/slac/g/glast/ground/cvs/CalRecon/CalRecon/IG4GenErrorSvc.h,v 1.2 2005/07/13 14:52:17 chamont Exp $
*/

class IG4GenErrorSvc : public virtual IInterface
{
public:

    //! retrieve Gaudi interface ID
    static const InterfaceID& interfaceID() { return IID_IG4GenErrorSvc ; }

    //! register errors
    virtual StatusCode handleError(const std::string & catcherName, const std::string & comment ) =0 ;
};

#endif



