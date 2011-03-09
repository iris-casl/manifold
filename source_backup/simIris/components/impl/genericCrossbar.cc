/*!
 * =====================================================================================
 *
 *       Filename:  GenericCrossbar.cc
 *
 *    Description:  This class is to be deprecated. 
 *
 *        Version:  1.0
 *        Created:  02/20/2010 02:24:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */

#ifndef  _genericcrossbar_cc_INC
#define  _genericcrossbar_cc_INC

#include	"genericCrossbar.h"

GenericCrossbar::GenericCrossbar ()
{
    input_ports = 0;
    output_ports = 0;
}		/* -----  end of method GenericCrossbar::GenericCrossbar  ----- */

GenericCrossbar::~GenericCrossbar ()
{
    return;
}		/* -----  end of method GenericCrossbar::~GenericCrossbar  ----- */

void
GenericCrossbar::clear ()
{
    return ;
}		/* -----  end of method GenericCrossbar::clear  ----- */

uint
GenericCrossbar::get_no_input_ports ()
{
    return input_ports;
}		/* -----  end of method GenericCrossbar::get_output_ports  ----- */

uint
GenericCrossbar::get_no_output_ports ()
{
    return output_ports;
}		/* -----  end of method GenericCrossbar::get_output_ports  ----- */

void
GenericCrossbar::setup (uint p, uint v)
{
    vcs = v;
    input_ports =p;
    output_ports =p;

    map.resize ( vcs );
    busy.resize ( vcs );

    for ( uint i = 0; i < vcs ; i++)
    { 
        map[i].resize( input_ports );
        busy[i].resize( output_ports );
    }

    for ( uint i = 0; i < vcs ; i++)
        for ( uint j = 0; j < input_ports ; j++ )
        {
            map[i][j] = 0;
            busy[i][j] = false;
        } 

    return ;
}		/* -----  end of method GenericCrossbar::set_no_channels  ----- */

uint
GenericCrossbar::get_no_channels ()
{
    return map.size();
}		/* -----  end of method GenericCrossbar::get_no_channels  ----- */

uint
GenericCrossbar::get_map ( uint port, uint ch)
{
   
    if ( port >= input_ports )
    {
        cout << "Tried to get the mapping from input port " << port << " but the crossbar only has "  << input_ports << " input ports.\n";
        cout << " Need to throw an invalid port exception " << endl;
    }
    return map[ch][port];
}		/* -----  end of method GenericCrossbar::get_map  ----- */

void
GenericCrossbar::configure_crossbar ( uint inport, uint outport, uint och )
{
    map[och][outport] = inport;
    busy[och][outport] = true;
    return ;
}		/* -----  end of method GenericCrossbar::configure_crossbar  ----- */

void
GenericCrossbar::push ( uint outport, uint och)
{
    busy[och][outport] = true;
    return ;
}		/* -----  end of method GenericCrossbar::push  ----- */

void
GenericCrossbar::pull ( uint outport, uint och )
{
    busy[och][outport] = false;
    return ;
}		/* -----  end of method GenericCrossbar::pull  ----- */

bool
GenericCrossbar::is_full ( uint inport, uint och )
{
    for ( uint i=0; i<busy.size(); i++)
        if ( map[och][i] == inport)
            return true;

    return false;
}		/* -----  end of method GenericCrossbar::full  ----- */

bool
GenericCrossbar::is_empty ( uint oport, uint ch )
{
    if( ch>= busy.size() )
        cout << "Invalid channel " << endl;

    if(oport >= busy[0].size())
        cout << "Invalid output port" << busy[0].size() << endl;

    return !busy[ch][oport];
}		/* -----  end of method GenericCrossbar::is_port_empty  ----- */

string
GenericCrossbar::toString () const
{
    stringstream str;
    str << "GenericCrossbar"
        << "\t channels: " << map.size()
        << "\t ports: " << map[0].size()
        << "\t output_ports: " << output_ports
        << endl;
    return str.str();
}		/* -----  end of function GenericCrossbar::toString  ----- */
#endif   /* ----- #ifndef _genericcrossbar_cc_INC  ----- */

