/*
 * =====================================================================================
 *
 *       Filename:  event.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/07/2010 05:28:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mitchelle Rasquinha (), mitchelle.rasquinha@gatech.edu
 *        Company:  Georgia Institute of Technology
 *
 * =====================================================================================
 */


#ifndef  event_h_INC
#define  event_h_INC

#include	<vector>
#include	<sstream>
#include	<string>
#include	"../../../kernel/component.h"
#include	"../../../util/simIrisComponentHeader.h"

using namespace std;

/*
 * =====================================================================================
 *        Class:  IrisEvent
 *  Description:  
 *
 * =====================================================================================
 */
class IrisEvent
{
    public:
        IrisEvent();
        ~IrisEvent();
        uint src_id;
        uint dst_id;
	Component* src;
	Component* dst;
        vector<void *> event_data;
        simTime time;
        IrisEventType type;
        uint vc;
        string toString(void);

    protected:

    private:

}; /* -----  end of class IrisEvent  ----- */

#endif   /* ----- #ifndef event_h_INC  ----- */
