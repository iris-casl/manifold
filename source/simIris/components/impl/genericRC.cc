/*
 * =====================================================================================
 *
 *       Filename:  genericRC.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2010 12:13:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */
#ifndef  _genericaddressdecoder_cc_INC
#define  _genericaddressdecoder_cc_INC

#include	"genericRC.h"

using namespace std;

GenericRC::GenericRC()
{
    name = "AddrDecoder";
    node_ip = 0;
    address = 0;
}

uint
GenericRC::route_x_y(uint dest)
{
    uint oport = -1;
    uint myx=-1, destx=-1, myy =-1, desty=-1;
    myx = grid_xloc[node_ip];
    myy = grid_yloc[node_ip];
    destx = grid_xloc[ dest ];
    desty = grid_yloc[ dest ];
    if ( myx == destx && myy == desty )
        oport = 0;
    else if ( myx ==  destx )
    {
        if( desty < myy )
            oport = 3;
        else
            oport = 4;
    }
    else
    {
        if( destx < myx )
            oport = 1;
        else
            oport = 2;
    }

#ifdef _DEBUG
    _DBG(" Route HEAD dest: %d oport: %d ", dest, oport);
    _DBG(" addr: %d myid_x: %d myid_y: %d destid_x: %d destid_y: %d", node_ip, myx, myy, destx, desty);
#endif

    return oport;
}

void
GenericRC::route_negative_first(HeadFlit* hf)
{
    uint dest = hf->dst_address;
    uint oport = -1;
    uint myx=-1, destx=-1, myy =-1, desty=-1;
    myx = grid_xloc[node_ip];
    myy = grid_yloc[node_ip];
    destx = grid_xloc[ dest ];
    desty = grid_yloc[ dest ];
    int deltax = destx - myx;
    int deltay = desty - myy;
    if( deltax == 0 && deltay == 0 )
    {
//        _DBG_NOARG(" Reached dest!" );
        possible_out_ports.push_back(0);
        return;
    }

    if ( deltax < 0 && deltay < 0 )
    {
//        _DBG( " Both negative by x:%d y:%d", deltax, deltay );
        possible_out_ports.push_back(1);
        possible_out_ports.push_back(3);
        return;
    }
    else if ( deltax < 0 )
    {
//        _DBG( " x negative by %d ", deltax);
        possible_out_ports.push_back(1);
        return;
    }
    else if ( deltay < 0 )
    {
//        _DBG( " y negative by %d ", deltay );
        possible_out_ports.push_back(3);
        return;
    }
    else if (deltax > 0 && deltay > 0 ) 
    {
//        _DBG( " Both positive by x:%d y;%d", deltax, deltay );
        possible_out_ports.push_back(2);
        possible_out_ports.push_back(4);
        return;
    }
    else if ( deltax == 0)
    {
        possible_out_ports.push_back(4);
        return;
    }
    else if( deltay == 0)
    {
        possible_out_ports.push_back(2);
        return;
    }
    else
    {
        _DBG(" ERROR: Some condition not covered deltax: %d , deltay:%d", deltax, deltay);
    }
        return;
}

void
GenericRC::route_west_first(HeadFlit* hf)
{
    uint dest = hf->dst_address;
    uint oport = -1;
    uint myx=-1, destx=-1, myy =-1, desty=-1;
    myx = grid_xloc[node_ip];
    myy = grid_yloc[node_ip];
    destx = grid_xloc[ dest ];
    desty = grid_yloc[ dest ];
    int deltax = destx - myx;
    int deltay = desty - myy;
    if( deltax == 0 && deltay == 0 )
    {
        possible_out_ports.push_back(0);
        return;
    }

    if ( deltax < 0 )
    {
            possible_out_ports.push_back(1);
            return;
    }
    else if ( deltax > 0 )
    {
        if ( deltay < 0)
        {
            possible_out_ports.push_back(2);
            possible_out_ports.push_back(3);
            return;
        }
        else if ( deltay > 0)
        {
            possible_out_ports.push_back(2);
            possible_out_ports.push_back(4);
            return;
        }
        else
        {
            possible_out_ports.push_back(2);
            return;
        }
    }
    else if ( deltax == 0 )
    {
        if ( deltay < 0)
        {
            possible_out_ports.push_back(3);
            return;
        }
        else if ( deltay > 0)
        {
            possible_out_ports.push_back(4);
            return;
        }
        else
        {
            cerr << "\nERROR should have exited earlier " << endl;
        }
    }
    else
    {
        _DBG(" ERROR: Some condition not covered deltax: %d , deltay:%d", deltax, deltay);
    }
        return;
}

void
GenericRC::route_north_last(HeadFlit* hf)
{
    uint dest = hf->dst_address;
    uint oport = -1;
    uint myx=-1, destx=-1, myy =-1, desty=-1;
    myx = grid_xloc[node_ip];
    myy = grid_yloc[node_ip];
    destx = grid_xloc[ dest ];
    desty = grid_yloc[ dest ];
    int deltax = destx - myx;
    int deltay = desty - myy;
    if( deltax == 0 && deltay == 0 )
    {
        possible_out_ports.push_back(0);
        return;
    }
    if ( deltax < 0 )
    {
        if ( deltay < 0)
        {
            possible_out_ports.push_back(1);
            return;
        }
        else if ( deltay > 0 )
        {
            possible_out_ports.push_back(1);
            possible_out_ports.push_back(4);
            return;
        }
        else
        {
            possible_out_ports.push_back(1);
            return;
        }
    }
    else if ( deltax > 0 )
    {
        if ( deltay < 0)
        {
            possible_out_ports.push_back(2);
            return;
        }
        else if ( deltay > 0)
        {
            possible_out_ports.push_back(4);
            possible_out_ports.push_back(2);
            return;
        }
        else
        {
            possible_out_ports.push_back(2);
            return;
        }
    }
    else if ( deltax == 0 )
    {
        if ( deltay < 0)
        {
            possible_out_ports.push_back(3);
            return;
        }
        else if ( deltay > 0)
        {
            possible_out_ports.push_back(4);
            return;
        }
        else
        {
            cerr << "\nERROR should have exited earlier " << endl;
        }
    }

    else
    {
        _DBG(" ERROR: Some condition not covered deltax: %d , deltay:%d", deltax, deltay);
    }
        return;
}

void
GenericRC::route_north_last_non_minimal(HeadFlit* hf)
{
    uint dest = hf->dst_address;
    uint oport = -1;
    uint myx=-1, destx=-1, myy =-1, desty=-1;
    myx = grid_xloc[node_ip];
    myy = grid_yloc[node_ip];
    destx = grid_xloc[ dest ];
    desty = grid_yloc[ dest ];
    int deltax = destx - myx;
    int deltay = desty - myy;
    bool is_fringe_router = false;
    if ( node_ip%grid_size == 0 || (node_ip+1)%grid_size == 0 
        || (node_ip+grid_size) > no_nodes || ( node_ip-grid_size)<0)
        is_fringe_router = true;

    /* Allow all paths to fringe routers. However fringe routers follow DOR */
    if( is_fringe_router )
    {
            possible_out_ports.push_back(route_x_y(hf->dst_address));
            return;
    }

    if( deltax == 0 && deltay == 0 )
    {
        possible_out_ports.push_back(0);
        return;
    }
    else if( hf->inport == 4 )
    {
        possible_out_ports.push_back(3);
        return;
    }
    if ( deltax != 0 || (deltax == 0 && deltay > 0)  )
    {
        /* 
        bool dest_on_fringe = false;
        if ( dest%grid_size == 0 || (dest+1)%grid_size == 0 
             || (dest+grid_size) > no_nodes || ( dest-grid_size)<0)
             dest_on_fringe = true;

        if( dest_on_fringe )
        {
            possible_out_ports.push_back(route_x_y(hf->dst_address));
            return;
        }
         * */
//        cout << " Cannot add 3 inport:" << hf->inport <<endl;
        if( hf->inport != 1 )
        {
            possible_out_ports.push_back(1);
//        cout << " :Add 1" << endl;
        }
        if( hf->inport != 2)
        {
            possible_out_ports.push_back(2);
//        cout << " :Add 2" << endl;
    }
        if( hf->inport != 4)
        {
            possible_out_ports.push_back(4);
//        cout << " :Add 4" << endl;
        }
        return;
    }
    else if ( deltax == 0 && deltay < 0)
    {
        possible_out_ports.push_back(3);
//        cout << " Can add 3" << endl;
        /*
        for ( uint j=1; j<5; j++)
            if ( j != hf->inport )
                possible_out_ports.push_back(j);
        */
        return;
    }
    else
    {
        _DBG(" ERROR: Some condition not covered deltax: %d , deltay:%d", deltax, deltay);
    }
        return;
}

void
GenericRC::route_odd_even(HeadFlit* hf)
{
    uint src = hf->src_address;
    uint dest = hf->dst_address;
    uint oport = -1;
    uint c0=-1, d0=-1, c1=-1, d1=-1, s0=-1, s1=-1;
    c0 = grid_xloc[node_ip];
    c1 = grid_yloc[node_ip];
    d0 = grid_xloc[ dest ];
    d1 = grid_yloc[ dest ];
    s0 = grid_xloc[ src ];
    s1 = grid_yloc[ src ];
    int e0 = d0-c0;
    int e1 = d1-c1;

    if( e0 == 0 && e1 == 0 )
    {
        possible_out_ports.push_back(0);
        return;
    }

    if( e0 == 0 )
    {
        if( e1>0 )
            possible_out_ports.push_back(3);
        else
            possible_out_ports.push_back(4);
    }
    else if ( e0 > 0 )
    {
        if ( e1 == 0 )
            possible_out_ports.push_back(2);
        else
        {
            if ((c0%2) != 0 || c0==s0 )
            {
                if ( e1 > 0 )
                    possible_out_ports.push_back(3);
                else
                    possible_out_ports.push_back(4);
            }

            if ( (d0%2)!=0 || e0 != 1 )
                possible_out_ports.push_back(2);
        }
    }
    else
    {
        possible_out_ports.push_back(1);
        if ( (c0%2)==0 )
        {
            if ( e1 > 0 )
                possible_out_ports.push_back(3);
            else
                possible_out_ports.push_back(4);
        }
    }

   if( possible_out_ports.size() == 0 ) 
    {
        _DBG(" ERROR: Some condition not covered d0: %d , d1:%d", d0, d1);
    }
        return;
}

void
GenericRC::push (Flit* f, uint ch )
{
    if(ch > addresses.size())
        cout << "Invalid VC Exception " << endl;


    //Route the head
    if( f->type == HEAD )
    {
        HeadFlit* header = static_cast< HeadFlit* >( f );
        addresses[ch].last_adaptive_port = 0;
        addresses[ch].possible_out_ports.clear();
        addresses[ch].possible_out_vcs.clear();
        possible_out_ports.clear();
        possible_out_vcs.clear();
        addresses[ch].last_adaptive_port = 0;

        if( rc_method == NEGATIVE_FIRST )
        {
            possible_out_ports.clear();
            route_negative_first( header );
            addresses [ch].out_port = possible_out_ports.at(0);
//            _DBG(" In RC no of paths: %d from %d to %d at %d", possible_out_ports.size(), header->src_address, header->dst_address, node_ip);
            for ( uint i=0; i<possible_out_ports.size(); i++)
                addresses [ch].possible_out_ports.push_back(possible_out_ports[i]);

        }
        else if( rc_method == WEST_FIRST)
        {
            possible_out_ports.clear();
            route_west_first( header );
            addresses [ch].out_port = possible_out_ports.at(0);
//            _DBG(" In RC no of paths: %d from %d to %d at %d", possible_out_ports.size(), header->src_address, header->dst_address, node_ip);
            for ( uint i=0; i<possible_out_ports.size(); i++)
                addresses [ch].possible_out_ports.push_back(possible_out_ports[i]);

        }
        else if( rc_method == NORTH_LAST)
        {
            possible_out_ports.clear();
            route_north_last( header );
            addresses [ch].out_port = possible_out_ports.at(0);
//            _DBG(" In RC no of paths: %d from %d to %d at %d", possible_out_ports.size(), header->src_address, header->dst_address, node_ip);
            for ( uint i=0; i<possible_out_ports.size(); i++)
                addresses [ch].possible_out_ports.push_back(possible_out_ports[i]);

        }
        else if( rc_method == ODD_EVEN)
        {
            possible_out_ports.clear();
            route_odd_even( header );
            addresses [ch].out_port = possible_out_ports.at(0);
            for ( uint i=0; i<possible_out_ports.size(); i++)
                addresses [ch].possible_out_ports.push_back(possible_out_ports[i]);

        }
        else if( rc_method == NORTH_LAST_NON_MINIMAL)
        {
            possible_out_ports.clear();
            route_north_last_non_minimal( header );
            addresses [ch].out_port = possible_out_ports.at(0);
            for ( uint i=0; i<possible_out_ports.size(); i++)
                addresses [ch].possible_out_ports.push_back(possible_out_ports[i]);

        }
        else
        {
            addresses [ch].out_port = route_x_y(header->dst_address);
            addresses [ch].possible_out_ports.push_back(route_x_y(header->dst_address));
        }

        addresses [ch].channel = header->vc;

        /* 
         {
         if( header->msg_class == RESPONSE_PKT )
         addresses[ch].possible_out_vcs.push_back(1);
         else
         addresses[ch].possible_out_vcs.push_back(0);
         }
         else
         * */
        {
            for ( uint i=0;i<vcs;i++)
                addresses[ch].possible_out_vcs.push_back(i);

        }

        addresses [ch].route_valid = true;

#ifdef _DEEP_DEBUG
        _DBG(" computed oport %d %d dest: %d",addresses [ch].out_port, addresses [ch].channel, header->dst_address);
#endif

    }
    else if(f->type == TAIL)
    {
        if( !addresses[ch].route_valid)
        {
            _DBG_NOARG("TAIL InvalidAddrException" );
        }

        addresses[ch].route_valid = false;
        addresses[ch].possible_out_ports.clear();
        addresses[ch].possible_out_vcs.clear();
        addresses[ch].last_adaptive_port = 0;
        possible_out_ports.clear();
        possible_out_vcs.clear();
    }
    else if (f->type == BODY)
    {
        if( !addresses[ch].route_valid)
        {
            _DBG_NOARG("BODY InvalidAddrException" );
        }
    }
    else
    {
        _DBG(" InvalidFlitException fty: %d", f->type);
    }

    return ;
}		/* -----  end of method genericRC::push  ----- */


uint
GenericRC::get_output_port ( uint ch)
{
    uint oport = -1;
    if (addresses[ch].last_adaptive_port == addresses[ch].possible_out_ports.size())
        addresses[ch].last_adaptive_port = 0;
    oport  = addresses[ch].possible_out_ports[addresses[ch].last_adaptive_port];
    addresses[ch].last_adaptive_port++;

    return oport;
}		/* -----  end of method genericRC::get_output_port  ----- */

uint
GenericRC::no_adaptive_vcs( uint ch )
{
    return addresses[ch].possible_out_vcs.size();
}

uint
GenericRC::no_adaptive_ports( uint ch )
{
    return addresses[ch].possible_out_ports.size();
}

uint
GenericRC::get_virtual_channel ( uint ch )
{
    uint och = -1;
    if (addresses[ch].last_vc == addresses[ch].possible_out_vcs.size())
        addresses[ch].last_vc = 0;

    och = addresses[ch].possible_out_vcs[addresses[ch].last_vc];
    addresses[ch].last_vc++;
    
    return och;
}		/* -----  end of method genericRC::get_vc  ----- */

void
GenericRC::resize ( uint ch )
{
    vcs = ch;
    addresses.resize(ch);
    for ( uint i = 0 ; i<ch ; i++ )
    {
        addresses[i].route_valid = false;
	addresses[i].last_vc = 0;
    }
    return ;
}		/* -----  end of method genericRC::set_no_channels  ----- */

uint
GenericRC::get_no_channels()
{
    return addresses.size();
}		/* -----  end of method genericRC::get_no_channels  ----- */

bool
GenericRC::is_empty ()
{
    uint channels = addresses.size();
    for ( uint i=0 ; i<channels ; i++ )
        if(addresses[i].route_valid)
            return false;

    return true;
}		/* -----  end of method genericRC::is_empty  ----- */

string
GenericRC::toString () const
{
    stringstream str;
    str << "GenericRC"
        << "\tchannels: " << addresses.size();
    return str.str();
}		/* -----  end of function GenericRC::toString  ----- */
#endif   /* ----- #ifndef _genericaddressdecoder_cc_INC  ----- */

