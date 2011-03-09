/*
 * =====================================================================================
 *
 *       Filename:  link.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/22/2010 07:32:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _link_h_INC
#define  _link_h_INC

#include	"networkComponent.h"
#include	<string>

using namespace std;

/*
 * =====================================================================================
 *        Class:  IrisLink
 *  Description:  
 * =====================================================================================
 */
class IrisLink: public NetworkComponent
{
    public:
        IrisLink () {}                             /* constructor */
        ~IrisLink() {}
        NetworkComponent* input_connection;
        NetworkComponent* output_connection;
        string toString() const;
        virtual void process_event( IrisEvent* e) =0;

    protected:

    private:

}; /* -----  end of class IrisLink  ----- */

#endif   /* ----- #ifndef _link_h_INC  ----- */

