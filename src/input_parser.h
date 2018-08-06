/**
 *  @file    input_parser.h
 *  @author  Fabio Chiusano
 *  @date    08/06/2018
 *  @version 1.0
 */

#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

/**
 * @brief class whose goal is to parse the input arguments of the program
 *
 * @author iain (stackoverflow)
 */
class InputParser{
private:
		vector <string> tokens;
public:
  /**
   * @brief constructor of InputParser
   *
   * @param argc the argc of the main
   * @param argv the argv of the main
   */
    InputParser (int &argc, char **argv);

    /**
     * @brief returns the string in argv after the specified option
     *
     * @param  option string, the option to check (e.g. -d)
     * @return        the string after the specified option
     */
    const string& getCmdOption(const string &option) const;

    /**
     * @brief returns true if the specified option exists in ergv. Return false otherwise
     *
     * @param  option string, the option to check
     * @return        a boolean representing the presence of the specified option in argv
     */
    bool cmdOptionExists(const string &option) const;
};
