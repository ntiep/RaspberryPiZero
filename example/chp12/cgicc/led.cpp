#include <iostream>         // for the input/output
#include <stdlib.h>         // for the getenv call
#include <sys/sysinfo.h>    // for the system uptime call
#include <cgicc/Cgicc.h>    // the cgicc headers
#include <cgicc/CgiDefs.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <wiringPi.h>
#define LED_GPIO 17
using namespace std;
using namespace cgicc;

int main(){
   Cgicc form;                      // the CGI form object
   wiringPiSetupGpio();             // uses wiringPi - see Chp.6.
   pinMode(LED_GPIO, OUTPUT);       // GPIO17 is used as an output
   string cmd;                      // the Set LED command

   // get the state of the form that was submitted - script calls itself
   bool isStatus = form.queryCheckbox("status");
   form_iterator it = form.getElement("cmd");  // the radio command
   if (it == form.getElements().end() || it->getValue()==""){
      cmd = "off";                     // if it is invalid use "off"
   }
   else { cmd = it->getValue(); }      // otherwise use submitted value
   char *value = getenv("REMOTE_ADDR");    // The remote IP address
 
   // generate the form but use states that are set in the submitted form
   cout << HTTPHTMLHeader() << endl;       // Generate the HTML form
   cout << html() << head() << title("LED Example") << head() << endl;
   cout << body() << h1("Exploring RPi POST LED Example") << endl;;
   cout << "<form action=\"/cgi-bin/led.cgi\" method=\"POST\">\n";
   cout << "<div>Set LED: <input type=\"radio\" name=\"cmd\" value=\"on\""
        << ( cmd=="on" ? "checked":"") << "/> On ";
   cout << "<input type=\"radio\" name=\"cmd\" value=\"off\""
        << ( cmd=="off" ? "checked":"") << "/> Off ";
   cout << "<input type=\"submit\" value=\"Set LED\" />";
   cout << "</div></form>";
 
   // process the form data to change the LED state
   if (cmd=="on") digitalWrite(LED_GPIO, HIGH);          // turn on
   else if (cmd=="off") digitalWrite(LED_GPIO, LOW);     // turn off
   else cout << "<div> Invalid command! </div>";         // not possible
   cout << "<div> The CGI REMOTE_ADDR value is " << value << "</div>";
   cout << body() << html();
   return 0;
}
