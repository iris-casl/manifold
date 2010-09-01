/*
 * =====================================================================================
 *
 *       Filename:  processor.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 12:12:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _processor_h_INC
#define  _processor_h_INC

#include	"networkComponent.h"
#include	"../../data_types/impl/irisEvent.h"
#include	"interface.h"

/*
 * =====================================================================================
 *        Class:  Processor
 *  Description:  
 * =====================================================================================
 */
class Processor : public NetworkComponent
{
    public:
        Processor ();                             /* constructor */
        virtual ~Processor();
        vector < Interface* > interface_connections;
        void init();
        virtual string toString() const;
        virtual void process_event(IrisEvent* e) = 0;
        virtual void setup(uint n, uint v, uint time) = 0;
        virtual void set_output_path( string outpath_name ) = 0;
        virtual string print_stats() const = 0 ;
        bool ni_recv;
    protected:

    private:

}; /* -----  end of class Processor  ----- */

#endif   /* ----- #ifndef _processor_h_INC  ----- */

