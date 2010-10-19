/*!
 * =====================================================================================
 *
 *       Filename:  genericCrossbar.h
 *
 *    Description:  This class is to be deprecated. Its used with the physical
 *    router and tries to describe how a crossbar can be written as a
 *    component.
 *
 *        Version:  1.0
 *        Created:  02/20/2010 02:09:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericcrossbar_h_INC
#define  _genericcrossbar_h_INC

#include	"../interfaces/crossbar.h"
#include	<iostream>

using namespace std;

class GenericCrossbar: public Crossbar
{
    public:
        GenericCrossbar ();                             /* constructor */
        ~GenericCrossbar();
        void setup( uint ports, uint vcs );
        uint get_no_input_ports();
        uint get_no_output_ports (); 
        void set_no_virtual_channels ( uint no);
        uint get_no_channels ();
        uint get_map ( uint in_port, uint ch );
        void configure_crossbar( uint in_port, uint out_port, uint ch );
        void push (uint in_port, uint ch);
        void pull ( uint out_port, uint ch );
        bool is_full ( uint in_port, uint ch);
        bool is_empty(uint out_port, uint ch);
        string toString() const;

    protected:

    private:
        vector < vector<uint> > map;
        uint input_ports;
        uint output_ports;
        uint vcs;
        /*
         * =====================================================================================
         *        Class:  CrossbarUnit
         *  Description:  This class pairs a pointer to a flit with a bool
         *  indicating if it is valid.
         * =====================================================================================
         */
        class CrossbarUnit
        {
            public:
                Flit* data;
                bool valid;

            protected:

            private:

        }; /* -----  end of class CrossbarUnit  ----- */
        vector < vector<bool> > busy;
        void clear();

}; /* -----  end of class GenericCrossbar  ----- */

#endif   /* ----- #ifndef _genericcrossbar_h_INC  ----- */

