/*
 * =====================================================================================
 *
 *       Filename:  clock.h
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  02/06/2010 03:51:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include	"../kernel/simulator.h"

using namespace std;
/*
 * =====================================================================================
 *        Class:  Clock
 *  Description:  
 *
 * =====================================================================================
 */
class Clock
{
    public:
        Clock ();                             /* constructor */
        void updateTimestamp();

    protected:

    private:
        unsigned long int sim_timestamp;

}; /* -----  end of class Clock  ----- */

