//
// ============================================================================
// (C) Copyright Schalk W. Cronje 2013-2017
//
// This software is licensed under the Apache License 2.0
// See http://www.apache.org/licenses/LICENSE-2.0 for license details
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.
//
// ============================================================================
//

package org.ysb33r.gradle.doxygen.impl

import groovy.transform.CompileStatic
import groovy.transform.TupleConstructor
import org.gradle.api.logging.Logger
import org.ysb33r.gradle.doxygen.DoxygenException

import java.util.regex.Matcher

/**
 * Created by schalkc on 21/05/2014.
 */
@TupleConstructor
class DoxyfileEditor {

    Logger logger

    /** Scans for property names and replace them with updated values.
     * Removes all comments.
     * Any occurrences of += are removed if the property is updated
     *
     * If a supplied property is not found in the doxyfile it is logged as a warning
     *
     * @param doxyfile File to be edited
     * @param updates Properties to be edited in file
     */
    void update( def updates = [:], final File doxyfile ) {

        if( !updates.size() ) {
            return
        }

        // Read all lines
        Map properties = readProperties(doxyfile)
        updates.each { name,value ->
            if(properties[name] == null) {
                logger.warn "Ignoring '${name}' as it has not been found in the Doxyfile template"
            } else {
                properties[name] = value
            }
        }

        doxyfile.withPrintWriter { w ->
            properties.each { name,value ->
                w.println "${name} = ${value}"
            }
        }
    }

    /** Parses file, discards comments & blank lines.
     *
     * @param doxyfile - File to be parsed
     * @return A map of properties
     */
    private Map readProperties( final File doxyfile ) {
        Map result = [:]
        Integer lineCount
        boolean continuation = false
        String last
        doxyfile.eachLine { line,count ->
            if (continuation) {
                continuation=line.endsWith('\\')
                result[last]+= " ${trimContinuationMarker(line)}"
            } else {
                if( !( line.size() == 0 || line.startsWith('#') || line.matches(/^\s+$/)) ) {
                    Matcher matches = line =~ /^\s*([\p{Upper}\p{Digit}_]{3,})\s*(\+?\=)\s*(.*)$/

                    if(!matches || matches[0].size() != 4) {
                        throw new DoxygenException("Doxyfile parsing error ${doxyfile.absolutePath}:${count}")
                    }

                    continuation=line.endsWith('\\')
                    if(matches[0][2] == '+=' && result[matches[0][1]]) {
                        last = matches[0][1]
                        result[last]+= " ${trimContinuationMarker(matches[0][3])}"
                    } else {
                        last = matches[0][1]
                        result[last] = trimContinuationMarker(matches[0][3])
                    }
                }
            }
        }

        return result
    }

    @CompileStatic
    private String trimContinuationMarker(final String line) {
        if(line.endsWith('\\')) {
            if(line.size() == 1) {
                return ''
            } else if(line.size() == 2) {
                return line[0]
            } else {
                return line[0..-2]
            }
        } else {
            return line
        }
    }
}
