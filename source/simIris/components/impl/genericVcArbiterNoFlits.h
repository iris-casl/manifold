/*
 * =====================================================================================
 *
 *       Filename:  genericvcarbiternoflits.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/02/2010 01:35:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericvcarbiternoflits_h_INC
#define  _genericvcarbiternoflits_h_INC

#include	<vector>
#include	"../interfaces/arbiter.h"
#include	"../interfaces/genericComponentHeader.h"

/*
 * =====================================================================================
 *        Class:  genericvcarbiternoflitsNoFlits
 *  Description:  
 * =====================================================================================
 */
class genericvcarbiternoflitsNoFlits : public Arbiter
{
    public:
        genericvcarbiternoflitsNoFlits ();
        ~genericvcarbiternoflitsNoFlits();
        uint node_ip;
        uint address;
        string name;
        bool is_requested(uint ch);
        void set_no_vcs( uint ch);
        void request( uint ch);
        uint get_no_requests();
        uint pick_winner();
        string toString() const;
        void clear_winner();

    protected:

    private:
        uint vcs;
        vector < bool > requests;
        uint last_winner;
        bool done;

}; /* -----  end of class genericvcarbiternoflitsNoFlits  ----- */


#endif   /* ----- #ifndef _genericvcarbiternoflits_h_INC  ----- */


