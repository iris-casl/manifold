/*****************************************************************************
 *                                McPAT
 *                      SOFTWARE LICENSE AGREEMENT
 *            Copyright 2009 Hewlett-Packard Development Company, L.P.
 *                          All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Any User of the software ("User"), by accessing and using it, agrees to the
 * terms and conditions set forth herein, and hereby grants back to Hewlett-
 * Packard Development Company, L.P. and its affiliated companies ("HP") a
 * non-exclusive, unrestricted, royalty-free right and license to copy,
 * modify, distribute copies, create derivate works and publicly display and
 * use, any changes, modifications, enhancements or extensions made to the
 * software by User, including but not limited to those affording
 * compatibility with other hardware or software, but excluding pre-existing
 * software applications that may incorporate the software.  User further
 * agrees to use its best efforts to inform HP of any such changes,
 * modifications, enhancements or extensions.
 *
 * Correspondence should be provided to HP at:
 *
 * Director of Intellectual Property Licensing
 * Office of Strategy and Technology
 * Hewlett-Packard Company
 * 1501 Page Mill Road
 * Palo Alto, California  94304
 *
 * The software may be further distributed by User (but not offered for
 * sale or transferred for compensation) to third parties, under the
 * condition that such third parties agree to abide by the terms and
 * conditions of this license.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITH ANY AND ALL ERRORS AND DEFECTS
 * AND USER ACKNOWLEDGES THAT THE SOFTWARE MAY CONTAIN ERRORS AND DEFECTS.
 * HP DISCLAIMS ALL WARRANTIES WITH REGARD TO THE SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL
 * HP BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ***************************************************************************/
#include "io.h"
#include <iostream>
#include "xmlParser.h"
#include "XML_Parse.h"
#include "processor.h"
#include "globalvar.h"
#include "version.h"
#include "../util/config_params.h"


using namespace std;

void print_usage(char * argv0);

int main(int argc,char *argv[])
{
	char * fb ;
	bool infile_specified     = false;
	int  plevel               = 2;
	opt_for_clk	=true;
	//cout.precision(10);
	if (argc <= 1 || argv[1] == string("-h") || argv[1] == string("--help"))
	{
		print_usage(argv[0]);
	}

	for (int32_t i = 0; i < argc; i++)
	{
		if (argv[i] == string("-infile"))
		{
			infile_specified = true;
			i++;
			fb = argv[ i];
		}

		if (argv[i] == string("-print_level"))
		{
			i++;
			plevel = atoi(argv[i]);
		}

		if (argv[i] == string("-opt_for_clk"))
		{
			i++;
			opt_for_clk = (bool)atoi(argv[i]);
		}
	}
	if (infile_specified == false)
	{
		print_usage(argv[0]);
	}


	cout<<"McPAT (version "<< VER_MAJOR <<"."<< VER_MINOR
		<< " of " << VER_UPDATE << ") is computing the target processor...\n "<<endl;

	//parse XML-based interface
	ParseXML *p1= new ParseXML();
	p1->parse(fb);
        mcpat::Processor proc(p1);
        //proc.nocs[0]->nocdynp.flit_size = 256;
        proc.init();
	proc.displayEnergy(2, plevel);
        if( proc.nocs.size() >1)
            cout << "Nocs size: " << proc.nocs[0]->router->buffer.area.get_area()*1e-6*proc.nocs[0]->nocdynp.input_ports<< endl;
        delete p1;
        return 0;
}

void print_usage(char * argv0)
{
    cerr << "How to use McPAT:" << endl;
    cerr << "  mcpat -infile <input file name>  -print_level < level of details 0~5 >  -opt_for_clk < 0 (optimize for ED^2P only)/1 (optimzed for target clock rate)>"<< endl;
    //cerr << "    Note:default print level is at processor level, please increase it to see the details" << endl;
    exit(1);
}
