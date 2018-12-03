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

package org.ysb33r.gradle.doxygen

import org.gradle.api.DefaultTask
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.TaskAction
import org.gradle.process.ExecResult

/**
 * Allows for the creation of template files in a location. 'src/doxygen' is the default.
 */
class DoxygenTemplateFiles extends DefaultTask {

    /** Location where to generate files into. By default this will be src/doxygen.
     *
     */
    @Input
    File location = new File(project.projectDir,'src/doxygen')

    /** Prefix used for naming files. By default it is the name of the project.
     *
     */
    @Input
    String prefix = project.name

    /** Location of doxygen executable. By default the search path will be used to find it.
     *
     */
    @Input
    String doxygen = 'doxygen'

    @TaskAction
    void exec() {
        runDoxygen '-l', new File(location,prefix+'LayoutTemplate.xml').absolutePath
        runDoxygen '-w', 'rtf', new File(location,prefix+'Style.rtf').absolutePath
        runDoxygen '-w', 'html', new File(location,prefix+'Header.html').absolutePath,
                new File(location,prefix+'Footer.html').absolutePath,
                new File(location,prefix+'.css').absolutePath
        runDoxygen '-w', 'latex', new File(location,prefix+'Header.tex').absolutePath,
                new File(location,prefix+'Footer.tex').absolutePath,
                new File(location,prefix+'Style.tex').absolutePath
        runDoxygen '-e', 'rtf', new File(location,prefix+'Extensions.rtf').absolutePath
    }

    /** Runs the Doxygen executable
     *
     * @param cmdargs
     */
    private void runDoxygen(String... cmdargs) {

        cmdargs.add(doxyfile.absolutePath)
        ExecResult execResult = project.exec {

            executable  doxygen
            args        cmdargs
        }
    }
}
