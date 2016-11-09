#include <iostream>
#include <limits>

#include "Calculator.h"


using namespace std;

int main()
{
    Calculator c;

    c.start_session();

    cout << "Press ENTER to end program...";
    cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
    return 0;
}
